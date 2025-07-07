#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrl>
#include <QSslConfiguration>
#include "teamdata.h"

/**
 * @brief 网络管理器类 - 负责通过网络获取竞赛数据
 * 
 * 支持多种数据源：
 * - HTTP/HTTPS API 接口
 * - WebSocket 实时推送
 * - TCP/UDP 自定义协议
 * - 竞赛平台 API (如 DOMjudge, PC^2 等)
 */
class NetworkManager : public QObject
{
    Q_OBJECT

public:
    enum DataSource {
        HTTP_API,       // HTTP REST API
        WEBSOCKET,      // WebSocket 实时推送
        TCP_SOCKET,     // TCP 自定义协议
        UDP_MULTICAST,  // UDP 组播
        DOMJUDGE_API,   // DOMjudge 竞赛平台
        ICPC_TOOLS      // ICPC Tools 格式
    };

    enum ConnectionStatus {
        Disconnected,
        Connecting,
        Connected,
        Error
    };

    explicit NetworkManager(QObject *parent = nullptr);
    ~NetworkManager();

    // 配置网络连接
    void setDataSource(DataSource source);
    void setServerUrl(const QString &url);
    void setApiEndpoint(const QString &endpoint);
    void setAuthentication(const QString &username, const QString &password);
    void setApiKey(const QString &apiKey);
    void setRequestHeaders(const QMap<QString, QString> &headers);
    
    // 连接管理
    void connectToServer();
    void disconnectFromServer();
    bool isConnected() const;
    ConnectionStatus connectionStatus() const;
    
    // 数据获取
    void fetchAllTeams();
    void fetchTeamData(const QString &teamId);
    void fetchContestInfo();
    void fetchProblemList();
    
    // 实时更新配置
    void setRealTimeEnabled(bool enabled);
    void setUpdateInterval(int milliseconds);
    
    // 网络状态
    QString lastError() const;
    QDateTime lastUpdateTime() const;
    int networkLatency() const; // 网络延迟(ms)
    
    // 重连机制
    void setAutoReconnect(bool enabled);
    void setMaxRetries(int retries);
    void setRetryInterval(int seconds);

signals:
    // 数据信号
    void teamDataReceived(const QList<TeamData> &teams);
    void teamUpdated(const TeamData &team);
    void contestInfoReceived(const QJsonObject &info);
    void problemListReceived(const QStringList &problems);
    
    // 连接状态信号
    void connectionStatusChanged(ConnectionStatus status);
    void connected();
    void disconnected();
    void networkError(const QString &error);
    
    // 实时更新信号
    void realTimeUpdate(const TeamData &team);
    void rankingUpdated();
    
    // 网络统计信号
    void networkStatsUpdated(int latency, int bytesReceived);

public slots:
    void retryConnection();
    void refreshData();

private slots:
    // HTTP 响应处理
    void onHttpReplyFinished();
    void onHttpError(QNetworkReply::NetworkError error);
    void onSslErrors(const QList<QSslError> &errors);
    
    // 重连定时器
    void onRetryTimer();
    void onHeartbeatTimer();
    
    // 网络状态监控
    void onNetworkStatusChanged();

private:
    // 网络组件
    QNetworkAccessManager *m_networkManager;
    QNetworkReply *m_currentReply;
    
    // 配置参数
    DataSource m_dataSource;
    QString m_serverUrl;
    QString m_apiEndpoint;
    QString m_username;
    QString m_password;
    QString m_apiKey;
    QMap<QString, QString> m_requestHeaders;
    
    // 连接状态
    ConnectionStatus m_connectionStatus;
    QString m_lastError;
    QDateTime m_lastUpdateTime;
    bool m_realTimeEnabled;
    int m_updateInterval;
    
    // 重连机制
    bool m_autoReconnect;
    int m_maxRetries;
    int m_currentRetries;
    int m_retryInterval;
    QTimer *m_retryTimer;
    QTimer *m_heartbeatTimer;
    
    // 网络统计
    int m_networkLatency;
    qint64 m_bytesReceived;
    QDateTime m_lastPingTime;
    
    // 辅助方法
    QNetworkRequest createRequest(const QString &endpoint) const;
    void setupSslConfiguration(QNetworkRequest &request) const;
    QList<TeamData> parseTeamData(const QJsonDocument &document) const;
    TeamData parseTeamJson(const QJsonObject &teamObj) const;
    void updateConnectionStatus(ConnectionStatus status);
    void handleNetworkError(const QString &error);
    void startHeartbeat();
    void stopHeartbeat();
    void measureLatency();
    
    // 不同数据源的解析方法
    QList<TeamData> parseDOMjudgeFormat(const QJsonDocument &document) const;
    QList<TeamData> parseICPCToolsFormat(const QJsonDocument &document) const;
    QList<TeamData> parseCustomFormat(const QJsonDocument &document) const;
};

#endif // NETWORKMANAGER_H
