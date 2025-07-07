#include "networkmanager.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkReply>
#include <QSslConfiguration>
#include <QTimer>
#include <QDebug>
#include <QUrlQuery>
#include <QAuthenticator>

NetworkManager::NetworkManager(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_currentReply(nullptr)
    , m_dataSource(HTTP_API)
    , m_connectionStatus(Disconnected)
    , m_realTimeEnabled(true)
    , m_updateInterval(5000) // 默认5秒更新
    , m_autoReconnect(true)
    , m_maxRetries(3)
    , m_currentRetries(0)
    , m_retryInterval(5) // 5秒重试间隔
    , m_retryTimer(new QTimer(this))
    , m_heartbeatTimer(new QTimer(this))
    , m_networkLatency(0)
    , m_bytesReceived(0)
{
    // 配置重试定时器
    m_retryTimer->setSingleShot(true);
    connect(m_retryTimer, &QTimer::timeout, this, &NetworkManager::onRetryTimer);
    
    // 配置心跳定时器
    m_heartbeatTimer->setInterval(30000); // 30秒心跳
    connect(m_heartbeatTimer, &QTimer::timeout, this, &NetworkManager::onHeartbeatTimer);
}

NetworkManager::~NetworkManager()
{
    disconnectFromServer();
}

void NetworkManager::setDataSource(DataSource source)
{
    m_dataSource = source;
}

void NetworkManager::setServerUrl(const QString &url)
{
    m_serverUrl = url;
}

void NetworkManager::setApiEndpoint(const QString &endpoint)
{
    m_apiEndpoint = endpoint;
}

void NetworkManager::setAuthentication(const QString &username, const QString &password)
{
    m_username = username;
    m_password = password;
}

void NetworkManager::setApiKey(const QString &apiKey)
{
    m_apiKey = apiKey;
}

void NetworkManager::setRequestHeaders(const QMap<QString, QString> &headers)
{
    m_requestHeaders = headers;
}

void NetworkManager::connectToServer()
{
    if (m_connectionStatus == Connecting || m_connectionStatus == Connected) {
        return;
    }
    
    updateConnectionStatus(Connecting);
    m_currentRetries = 0;
    
    // 测试连接
    measureLatency();
    fetchAllTeams();
}

void NetworkManager::disconnectFromServer()
{
    if (m_currentReply) {
        m_currentReply->abort();
        m_currentReply = nullptr;
    }
    
    stopHeartbeat();
    m_retryTimer->stop();
    updateConnectionStatus(Disconnected);
}

bool NetworkManager::isConnected() const
{
    return m_connectionStatus == Connected;
}

NetworkManager::ConnectionStatus NetworkManager::connectionStatus() const
{
    return m_connectionStatus;
}

void NetworkManager::fetchAllTeams()
{
    if (m_serverUrl.isEmpty()) {
        handleNetworkError("服务器URL未配置");
        return;
    }
    
    QString endpoint = m_apiEndpoint.isEmpty() ? "/api/teams" : m_apiEndpoint;
    QNetworkRequest request = createRequest(endpoint);
    
    m_currentReply = m_networkManager->get(request);
    
    connect(m_currentReply, &QNetworkReply::finished,
            this, &NetworkManager::onHttpReplyFinished);
    connect(m_currentReply, &QNetworkReply::errorOccurred,
            this, &NetworkManager::onHttpError);
    connect(m_currentReply, &QNetworkReply::sslErrors,
            this, &NetworkManager::onSslErrors);
    
    qDebug() << "正在获取队伍数据:" << request.url();
}

void NetworkManager::fetchTeamData(const QString &teamId)
{
    QString endpoint = QString("/api/teams/%1").arg(teamId);
    QNetworkRequest request = createRequest(endpoint);
    
    QNetworkReply *reply = m_networkManager->get(request);
    
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        reply->deleteLater();
        
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            TeamData team = parseTeamJson(doc.object());
            emit teamUpdated(team);
        } else {
            qWarning() << "获取队伍数据失败:" << reply->errorString();
        }
    });
}

void NetworkManager::fetchContestInfo()
{
    QNetworkRequest request = createRequest("/api/contest");
    
    QNetworkReply *reply = m_networkManager->get(request);
    
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        reply->deleteLater();
        
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            emit contestInfoReceived(doc.object());
        }
    });
}

void NetworkManager::fetchProblemList()
{
    QNetworkRequest request = createRequest("/api/problems");
    
    QNetworkReply *reply = m_networkManager->get(request);
    
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        reply->deleteLater();
        
        if (reply->error() == QNetworkReply::NoError) {
            QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
            QStringList problems;
            
            QJsonArray array = doc.array();
            for (const auto &value : array) {
                QJsonObject obj = value.toObject();
                problems.append(obj["id"].toString());
            }
            
            emit problemListReceived(problems);
        }
    });
}

void NetworkManager::setRealTimeEnabled(bool enabled)
{
    m_realTimeEnabled = enabled;
    
    if (enabled && isConnected()) {
        startHeartbeat();
    } else {
        stopHeartbeat();
    }
}

void NetworkManager::setUpdateInterval(int milliseconds)
{
    m_updateInterval = qMax(1000, milliseconds); // 最小1秒
    m_heartbeatTimer->setInterval(m_updateInterval);
}

QString NetworkManager::lastError() const
{
    return m_lastError;
}

QDateTime NetworkManager::lastUpdateTime() const
{
    return m_lastUpdateTime;
}

int NetworkManager::networkLatency() const
{
    return m_networkLatency;
}

void NetworkManager::setAutoReconnect(bool enabled)
{
    m_autoReconnect = enabled;
}

void NetworkManager::setMaxRetries(int retries)
{
    m_maxRetries = qMax(0, retries);
}

void NetworkManager::setRetryInterval(int seconds)
{
    m_retryInterval = qMax(1, seconds);
}

void NetworkManager::retryConnection()
{
    if (m_currentRetries < m_maxRetries) {
        m_currentRetries++;
        qDebug() << QString("重试连接 (%1/%2)").arg(m_currentRetries).arg(m_maxRetries);
        connectToServer();
    } else {
        handleNetworkError("达到最大重试次数，连接失败");
    }
}

void NetworkManager::refreshData()
{
    if (isConnected()) {
        fetchAllTeams();
    }
}

void NetworkManager::onHttpReplyFinished()
{
    if (!m_currentReply) {
        return;
    }
    
    QNetworkReply *reply = m_currentReply;
    m_currentReply = nullptr;
    
    if (reply->error() == QNetworkReply::NoError) {
        // 成功接收数据
        QByteArray data = reply->readAll();
        m_bytesReceived += data.size();
        m_lastUpdateTime = QDateTime::currentDateTime();
        
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QList<TeamData> teams;
        
        // 根据数据源格式解析
        switch (m_dataSource) {
        case DOMJUDGE_API:
            teams = parseDOMjudgeFormat(doc);
            break;
        case ICPC_TOOLS:
            teams = parseICPCToolsFormat(doc);
            break;
        default:
            teams = parseCustomFormat(doc);
            break;
        }
        
        emit teamDataReceived(teams);
        emit rankingUpdated();
        
        // 更新连接状态
        if (m_connectionStatus != Connected) {
            updateConnectionStatus(Connected);
            startHeartbeat();
        }
        
        // 重置重试计数
        m_currentRetries = 0;
        
        qDebug() << QString("成功获取 %1 支队伍数据，网络延迟: %2ms")
                    .arg(teams.size()).arg(m_networkLatency);
        
    } else {
        // 处理错误
        handleNetworkError(reply->errorString());
    }
    
    reply->deleteLater();
}

void NetworkManager::onHttpError(QNetworkReply::NetworkError error)
{
    Q_UNUSED(error)
    
    if (m_currentReply) {
        QString errorMsg = m_currentReply->errorString();
        handleNetworkError(errorMsg);
    }
}

void NetworkManager::onSslErrors(const QList<QSslError> &errors)
{
    QString errorMsg = "SSL错误: ";
    for (const auto &error : errors) {
        errorMsg += error.errorString() + "; ";
    }
    
    handleNetworkError(errorMsg);
    
    // 在开发环境中可以忽略SSL错误（生产环境中不建议）
    if (m_currentReply) {
        m_currentReply->ignoreSslErrors();
    }
}

void NetworkManager::onRetryTimer()
{
    retryConnection();
}

void NetworkManager::onHeartbeatTimer()
{
    if (m_realTimeEnabled) {
        refreshData();
    }
}

void NetworkManager::measureLatency()
{
    m_lastPingTime = QDateTime::currentDateTime();
    
    // 发送ping请求测量延迟
    QNetworkRequest request = createRequest("/api/ping");
    QNetworkReply *reply = m_networkManager->get(request);
    
    connect(reply, &QNetworkReply::finished, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            m_networkLatency = m_lastPingTime.msecsTo(QDateTime::currentDateTime());
            emit networkStatsUpdated(m_networkLatency, m_bytesReceived);
        }
        reply->deleteLater();
    });
}

QList<TeamData> NetworkManager::parseDOMjudgeFormat(const QJsonDocument &document) const
{
    QList<TeamData> teams;
    QJsonArray teamsArray = document.array();
    
    for (const auto &value : teamsArray) {
        QJsonObject teamObj = value.toObject();
        teams.append(parseTeamJson(teamObj));
    }
    
    return teams;
}

QList<TeamData> NetworkManager::parseICPCToolsFormat(const QJsonDocument &document) const
{
    // ICPC Tools 格式解析
    QList<TeamData> teams;
    QJsonObject root = document.object();
    QJsonArray teamsArray = root["teams"].toArray();
    
    for (const auto &value : teamsArray) {
        QJsonObject teamObj = value.toObject();
        TeamData team;
        team.fromJson(teamObj);  // 使用fromJson方法
        teams.append(team);
    }
    
    return teams;
}

QList<TeamData> NetworkManager::parseCustomFormat(const QJsonDocument &document) const
{
    QList<TeamData> teams;
    
    if (document.isArray()) {
        QJsonArray teamsArray = document.array();
        for (const auto &value : teamsArray) {
            teams.append(parseTeamJson(value.toObject()));
        }
    } else if (document.isObject()) {
        QJsonObject root = document.object();
        if (root.contains("teams")) {
            QJsonArray teamsArray = root["teams"].toArray();
            for (const auto &value : teamsArray) {
                teams.append(parseTeamJson(value.toObject()));
            }
        }
    }
    
    return teams;
}

QNetworkRequest NetworkManager::createRequest(const QString &endpoint) const
{
    QUrl url(m_serverUrl + endpoint);
    QNetworkRequest request(url);
    
    // 设置基本头部
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    request.setHeader(QNetworkRequest::UserAgentHeader, "RankingSystem/1.0");
    
    // 设置自定义头部
    for (auto it = m_requestHeaders.begin(); it != m_requestHeaders.end(); ++it) {
        request.setRawHeader(it.key().toUtf8(), it.value().toUtf8());
    }
    
    // 设置认证
    if (!m_apiKey.isEmpty()) {
        request.setRawHeader("Authorization", ("Bearer " + m_apiKey).toUtf8());
    } else if (!m_username.isEmpty() && !m_password.isEmpty()) {
        QString auth = m_username + ":" + m_password;
        QByteArray authData = auth.toUtf8().toBase64();
        request.setRawHeader("Authorization", ("Basic " + authData));
    }
    
    // 配置SSL
    setupSslConfiguration(request);
    
    return request;
}

void NetworkManager::setupSslConfiguration(QNetworkRequest &request) const
{
    QSslConfiguration config = QSslConfiguration::defaultConfiguration();
    config.setProtocol(QSsl::TlsV1_2OrLater);
    request.setSslConfiguration(config);
}

QList<TeamData> NetworkManager::parseTeamData(const QJsonDocument &document) const
{
    return parseCustomFormat(document);
}

void NetworkManager::updateConnectionStatus(ConnectionStatus status)
{
    if (m_connectionStatus != status) {
        m_connectionStatus = status;
        emit connectionStatusChanged(status);
        
        switch (status) {
        case Connected:
            emit connected();
            break;
        case Disconnected:
            emit disconnected();
            break;
        case Error:
            if (m_autoReconnect && m_currentRetries < m_maxRetries) {
                m_retryTimer->start(m_retryInterval * 1000);
            }
            break;
        default:
            break;
        }
    }
}

void NetworkManager::handleNetworkError(const QString &error)
{
    m_lastError = error;
    updateConnectionStatus(Error);
    emit networkError(error);
    
    qWarning() << "网络错误:" << error;
}

void NetworkManager::startHeartbeat()
{
    if (m_realTimeEnabled && !m_heartbeatTimer->isActive()) {
        m_heartbeatTimer->start();
    }
}

void NetworkManager::stopHeartbeat()
{
    m_heartbeatTimer->stop();
}

void NetworkManager::onNetworkStatusChanged()
{
    // 网络状态变化处理（替代弃用的API）
    qDebug() << "网络状态发生变化";
}

TeamData NetworkManager::parseTeamJson(const QJsonObject &teamObj) const
{
    // 使用fromJson方法解析
    TeamData team;
    team.fromJson(teamObj);
    return team;
}
