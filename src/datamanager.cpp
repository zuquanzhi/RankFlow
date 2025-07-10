#include "datamanager.h"
#include "binarysearchtree.h"
#include "networkmanager.h"  // 添加网络管理器头文件
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QStandardPaths>
#include <QCryptographicHash>
#include <QJsonDocument>
#include <algorithm>

DataManager::DataManager(QObject *parent)
    : QObject(parent)
    , m_refreshTimer(new QTimer(this))
    , m_fileWatcher(new QFileSystemWatcher(this))
    , m_queryTree(new TeamQueryTree(this))
    , m_networkManager(new NetworkManager(this))  // 初始化网络管理器
    , m_dataSource(LocalFile)                     // 默认本地文件
    , m_networkEnabled(false)                     // 默认禁用网络
{
    // 默认数据目录
    m_dataDirectory = "data";
    
    // 设置定时器
    m_refreshTimer->setSingleShot(false);
    connect(m_refreshTimer, &QTimer::timeout, this, &DataManager::onRefreshTimer);
    
    // 设置文件监视器
    connect(m_fileWatcher, &QFileSystemWatcher::fileChanged, 
            this, &DataManager::onFileChanged);
    
    // 连接网络管理器信号
    connect(m_networkManager, &NetworkManager::teamDataReceived,
            this, &DataManager::onNetworkDataReceived);
    connect(m_networkManager, &NetworkManager::networkError,
            this, &DataManager::onNetworkError);
    connect(m_networkManager, &NetworkManager::connected,
            this, &DataManager::onNetworkConnected);
    connect(m_networkManager, &NetworkManager::disconnected,
            this, &DataManager::onNetworkDisconnected);
    
    // 设置默认网络配置
    m_networkManager->setServerUrl("http://localhost:8080");
    m_networkManager->setApiEndpoint("/api/teams");
    m_networkManager->setDataSource(NetworkManager::HTTP_API);
    
    // 默认刷新间隔10分钟
    setRefreshInterval(600);
    
    // 连接查询树信号
    connect(m_queryTree, &TeamQueryTree::treeRebuilt, 
            this, [this](TeamQueryTree::SortCriteria criteria) {
        addAuditEntry(QString("查询树已重建，排序标准: %1").arg(static_cast<int>(criteria)));
    });
    
    connect(m_queryTree, &TeamQueryTree::teamAdded,
            this, [this](const QString& teamId) {
        addAuditEntry(QString("查询树中添加队伍: %1").arg(teamId));
    });
    
    connect(m_queryTree, &TeamQueryTree::teamRemoved,
            this, [this](const QString& teamId) {
        addAuditEntry(QString("查询树中移除队伍: %1").arg(teamId));
    });
}

void DataManager::setDataDirectory(const QString &path)
{
    if (m_dataDirectory != path) {
        m_dataDirectory = path;
        
        // 确保目录存在
        QDir().mkpath(m_dataDirectory);
        
        updateFileWatcher();
        addAuditEntry(QString("数据目录更改为: %1").arg(path));
    }
}

void DataManager::setRefreshInterval(int seconds)
{
    m_refreshTimer->setInterval(seconds * 1000);
    addAuditEntry(QString("刷新间隔设置为: %1秒").arg(seconds));
}

void DataManager::setAutoRefresh(bool enabled)
{
    if (enabled) {
        startAutoRefresh();
    } else {
        stopAutoRefresh();
    }
}

void DataManager::refreshData()
{
    emit refreshStarted();
    addAuditEntry("开始数据刷新");
    
    switch (m_dataSource) {
    case Network:
        refreshFromNetwork();
        break;
    case LocalFile:
        refreshFromLocal();
        break;
    case Hybrid:
        // 混合模式：同时从本地和网络获取数据
        refreshFromLocal(); // 先从本地加载
        
        // 如果网络可用，同时从网络获取数据，合并后会更新显示
        if (m_networkEnabled && m_networkManager->isConnected()) {
            refreshFromNetwork();
        }
        break;
    }
}

bool DataManager::loadTeamData(const QString &teamId)
{
    QString filePath = QString("%1/%2_results.json").arg(m_dataDirectory, teamId);
    
    if (loadTeamFromFile(filePath)) {
        emit teamDataChanged(teamId);
        addAuditEntry(QString("载入队伍数据: %1").arg(teamId));
        return true;
    }
    
    return false;
}

TeamData DataManager::getTeam(const QString &teamId) const
{
    for (const auto &team : m_teams) {
        if (team.teamId() == teamId) {
            return team;
        }
    }
    return TeamData();
}

QStringList DataManager::availableProblems() const
{
    QStringList problems;
    
    for (const auto &team : m_teams) {
        for (const auto &submission : team.submissions()) {
            if (!problems.contains(submission.problemId)) {
                problems.append(submission.problemId);
            }
        }
    }
    
    problems.sort();
    return problems;
}

void DataManager::logOperation(const QString &operation)
{
    addAuditEntry(operation);
}

QStringList DataManager::getAuditLog() const
{
    return m_auditLog;
}

void DataManager::startAutoRefresh()
{
    if (!m_refreshTimer->isActive()) {
        m_refreshTimer->start();
        addAuditEntry("自动刷新已启动");
    }
}

void DataManager::stopAutoRefresh()
{
    if (m_refreshTimer->isActive()) {
        m_refreshTimer->stop();
        addAuditEntry("自动刷新已停止");
    }
}

void DataManager::onRefreshTimer()
{
    refreshData();
}

void DataManager::onFileChanged(const QString &path)
{
    Q_UNUSED(path)
    // 文件改变时触发数据刷新
    QTimer::singleShot(1000, this, &DataManager::refreshData); // 延迟1秒避免重复触发
}

bool DataManager::loadAllTeams()
{
    QStringList teamFiles = findTeamFiles();
    QList<TeamData> newTeams;
    
    for (const QString &filePath : teamFiles) {
        TeamData team;
        if (team.loadFromFile(filePath)) {
            // 验证文件完整性
            if (verifyFileIntegrity(filePath)) {
                newTeams.append(team);
            } else {
                qDebug() << "文件完整性验证失败:" << filePath;
                emit errorOccurred(QString("文件完整性验证失败: %1").arg(filePath));
            }
        } else {
            qDebug() << "载入队伍数据失败:" << filePath;
        }
    }
    
    m_teams = newTeams;
    updateFileWatcher();
    
    // 重建查询树
    rebuildQueryTree();
    
    return true;
}

bool DataManager::loadTeamFromFile(const QString &filePath)
{
    TeamData team;
    if (!team.loadFromFile(filePath)) {
        return false;
    }
    
    if (!verifyFileIntegrity(filePath)) {
        emit errorOccurred(QString("文件完整性验证失败: %1").arg(filePath));
        return false;
    }
    
    // 更新或添加队伍数据
    bool found = false;
    for (int i = 0; i < m_teams.size(); ++i) {
        if (m_teams[i].teamId() == team.teamId()) {
            m_teams[i] = team;
            found = true;
            break;
        }
    }
    
    if (!found) {
        m_teams.append(team);
    }
    
    return true;
}

bool DataManager::verifyFileIntegrity(const QString &jsonPath)
{
    QString hashPath = jsonPath + ".sha256";  // 修复：直接添加扩展名
    
    QFileInfo hashFile(hashPath);
    if (!hashFile.exists()) {
        // 如果没有校验文件，跳过验证
        return true;
    }
    
    // 直接读取并验证文件，不调用updateStatistics()
    QFile file(jsonPath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError) {
        return false;
    }
    
    // 读取存储的哈希
    QFile hashFileObj(hashPath);
    if (!hashFileObj.open(QIODevice::ReadOnly)) {
        return false;
    }
    QString storedHash = QString::fromUtf8(hashFileObj.readAll()).trimmed();
    hashFileObj.close();
    
    // 计算紧凑格式的哈希
    QByteArray compactData = doc.toJson(QJsonDocument::Compact);
    QString calculatedHash = QString(QCryptographicHash::hash(compactData, QCryptographicHash::Sha256).toHex());
    
    bool isValid = (storedHash == calculatedHash);
    if (!isValid) {
        qDebug() << "文件完整性验证失败:" << jsonPath;
        qDebug() << "存储哈希:" << storedHash;
        qDebug() << "计算哈希:" << calculatedHash;
    }
    
    return isValid;
}

QStringList DataManager::findTeamFiles() const
{
    QDir dataDir(m_dataDirectory);
    if (!dataDir.exists()) {
        return QStringList();
    }
    
    QStringList filters;
    filters << "*_results.json";
    
    QStringList fileNames = dataDir.entryList(filters, QDir::Files);
    QStringList fullPaths;
    
    for (const QString &fileName : fileNames) {
        fullPaths.append(dataDir.absoluteFilePath(fileName));
    }
    
    return fullPaths;
}

void DataManager::updateFileWatcher()
{
    // 清除现有监视
    if (!m_fileWatcher->files().isEmpty()) {
        m_fileWatcher->removePaths(m_fileWatcher->files());
    }
    
    // 添加数据目录中的所有JSON文件到监视列表
    QStringList teamFiles = findTeamFiles();
    if (!teamFiles.isEmpty()) {
        m_fileWatcher->addPaths(teamFiles);
    }
    
    // 监视数据目录本身
    if (QDir(m_dataDirectory).exists()) {
        m_fileWatcher->addPath(m_dataDirectory);
    }
}

void DataManager::addAuditEntry(const QString &entry)
{
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString logEntry = QString("[%1] %2").arg(timestamp, entry);
    
    m_auditLog.append(logEntry);
    
    // 限制日志数量，保留最新的1000条
    if (m_auditLog.size() > 1000) {
        m_auditLog.removeFirst();
    }
    
    // 写入日志文件
    QDir logDir("logs");
    if (!logDir.exists()) {
        logDir.mkpath(".");
    }
    
    QString logFilePath = QString("logs/audit_%1.log")
                         .arg(QDate::currentDate().toString("yyyy-MM-dd"));
    
    QFile logFile(logFilePath);
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
        logFile.write((logEntry + "\n").toUtf8());
    }
}

// 新增的二叉树查询功能实现

void DataManager::rebuildQueryTree()
{
    if (m_queryTree && !m_teams.isEmpty()) {
        // 默认按分数排序构建树
        m_queryTree->buildTree(m_teams, TeamQueryTree::ByTotalScore);
        addAuditEntry(QString("查询树重建完成，包含%1支队伍").arg(m_teams.size()));
    }
}

void DataManager::updateQueryTree()
{
    rebuildQueryTree();
}

QList<TeamData> DataManager::getTeamsSortedBy(TeamQueryTree::SortCriteria criteria)
{
    if (!m_queryTree) {
        return m_teams;
    }
    
    // 如果当前排序标准不同，重建树
    if (m_queryTree->currentCriteria() != criteria) {
        m_queryTree->buildTree(m_teams, criteria);
    }
    
    return m_queryTree->getAllTeams();
}

QList<TeamData> DataManager::getTopTeamsByScore(int count)
{
    if (!m_queryTree) {
        // 备选方案：手动排序
        QList<TeamData> sortedTeams = m_teams;
        std::sort(sortedTeams.begin(), sortedTeams.end(), 
                  [](const TeamData& a, const TeamData& b) {
            return a.totalScore() > b.totalScore();
        });
        return sortedTeams.mid(0, qMin(count, sortedTeams.size()));
    }
    
    return m_queryTree->getTopTeams(count);
}

QList<TeamData> DataManager::getBottomTeamsByScore(int count)
{
    if (!m_queryTree) {
        // 备选方案：手动排序
        QList<TeamData> sortedTeams = m_teams;
        std::sort(sortedTeams.begin(), sortedTeams.end(), 
                  [](const TeamData& a, const TeamData& b) {
            return a.totalScore() < b.totalScore();
        });
        return sortedTeams.mid(0, qMin(count, sortedTeams.size()));
    }
    
    return m_queryTree->getBottomTeams(count);
}

QList<TeamData> DataManager::getTeamsInScoreRange(int minScore, int maxScore)
{
    if (!m_queryTree) {
        // 备选方案：线性搜索
        QList<TeamData> result;
        for (const TeamData& team : m_teams) {
            if (team.totalScore() >= minScore && team.totalScore() <= maxScore) {
                result.append(team);
            }
        }
        return result;
    }
    
    return m_queryTree->getTeamsInScoreRange(minScore, maxScore);
}

QList<TeamData> DataManager::searchTeamsByName(const QString& namePattern)
{
    if (!m_queryTree) {
        // 备选方案：线性搜索
        QList<TeamData> result;
        for (const TeamData& team : m_teams) {
            if (team.teamName().contains(namePattern, Qt::CaseInsensitive)) {
                result.append(team);
            }
        }
        return result;
    }
    
    return m_queryTree->searchByName(namePattern);
}

QList<TeamData> DataManager::searchTeamsBySolvedProblems(int minSolved)
{
    if (!m_queryTree) {
        // 备选方案：线性搜索
        QList<TeamData> result;
        for (const TeamData& team : m_teams) {
            if (team.solvedProblems() >= minSolved) {
                result.append(team);
            }
        }
        return result;
    }
    
    return m_queryTree->searchBySolvedProblems(minSolved);
}

QList<TeamData> DataManager::searchTeamsByAccuracy(double minAccuracy)
{
    if (!m_queryTree) {
        // 备选方案：线性搜索
        QList<TeamData> result;
        for (const TeamData& team : m_teams) {
            if (team.accuracy() >= minAccuracy) {
                result.append(team);
            }
        }
        return result;
    }
    
    return m_queryTree->searchByAccuracy(minAccuracy);
}

int DataManager::getTeamRank(const QString& teamId) const
{
    // 按分数排序获取排名
    QList<TeamData> sortedTeams = m_teams;
    std::sort(sortedTeams.begin(), sortedTeams.end(), 
              [](const TeamData& a, const TeamData& b) {
        return a.totalScore() > b.totalScore();
    });
    
    for (int i = 0; i < sortedTeams.size(); ++i) {
        if (sortedTeams[i].teamId() == teamId) {
            return i + 1; // 排名从1开始
        }
    }
    
    return -1; // 未找到
}

double DataManager::getAverageScore() const
{
    if (m_teams.isEmpty()) {
        return 0.0;
    }
    
    int totalScore = 0;
    for (const TeamData& team : m_teams) {
        totalScore += team.totalScore();
    }
    
    return static_cast<double>(totalScore) / m_teams.size();
}

int DataManager::getMedianScore() const
{
    if (m_teams.isEmpty()) {
        return 0;
    }
    
    QList<int> scores;
    for (const TeamData& team : m_teams) {
        scores.append(team.totalScore());
    }
    
    std::sort(scores.begin(), scores.end());
    
    int size = scores.size();
    if (size % 2 == 0) {
        return (scores[size/2 - 1] + scores[size/2]) / 2;
    } else {
        return scores[size/2];
    }
}

// ==== 网络功能实现 ====

void DataManager::setDataSource(DataSource source)
{
    if (m_dataSource != source) {
        m_dataSource = source;
        addAuditEntry(QString("数据源已切换为: %1").arg(
            source == Network ? "网络" : source == LocalFile ? "本地文件" : "混合模式"));
        emit dataSourceChanged(source);
    }
}

void DataManager::setNetworkEnabled(bool enabled)
{
    m_networkEnabled = enabled;
    if (enabled && !m_networkManager->isConnected()) {
        m_networkManager->connectToServer();
        addAuditEntry("网络功能已启用");
    }
}

void DataManager::setServerUrl(const QString &url)
{
    m_networkManager->setServerUrl(url);
    addAuditEntry(QString("服务器URL设置为: %1").arg(url));
}

void DataManager::setApiEndpoint(const QString &endpoint)
{
    m_networkManager->setApiEndpoint(endpoint);
}

void DataManager::setApiAuthentication(const QString &username, const QString &password)
{
    m_networkManager->setAuthentication(username, password);
}

void DataManager::setApiKey(const QString &apiKey)
{
    m_networkManager->setApiKey(apiKey);
}

void DataManager::setNetworkUpdateInterval(int milliseconds)
{
    m_networkManager->setUpdateInterval(milliseconds);
}

void DataManager::refreshFromNetwork()
{
    if (m_networkManager->isConnected()) {
        m_networkManager->fetchAllTeams();
    } else {
        if (m_dataSource == Hybrid) {
            // 在混合模式下，如果已经加载了本地数据，无需再次加载
            if (m_teams.isEmpty()) {
                refreshFromLocal();
            } else {
                emit refreshFinished();
            }
        } else {
            emit errorOccurred("网络连接不可用");
            emit refreshFinished();
        }
    }
}

void DataManager::refreshFromLocal()
{
    if (loadAllTeams()) {
        m_lastRefreshTime = QDateTime::currentDateTime();
        emit dataRefreshed();
    } else {
        emit errorOccurred("本地数据加载失败");
    }
    emit refreshFinished();
}

void DataManager::fallbackToLocal()
{
    // 只有当当前没有数据（未从本地加载过）时才需要回退加载
    if (m_teams.isEmpty()) {
        addAuditEntry("网络获取失败，回退到本地文件");
        refreshFromLocal();
    } else {
        addAuditEntry("网络获取失败，保留已加载的本地数据");
        emit refreshFinished();
    }
}

void DataManager::onNetworkDataReceived(const QList<TeamData> &teams)
{
    // 根据数据源模式决定如何处理网络数据
    if (m_dataSource == Hybrid) {
        // 在混合模式下，合并本地和网络数据而不是替换
        QHash<QString, TeamData> teamMap;
        
        // 先把现有数据（本地加载的）放入映射
        for (const TeamData &team : m_teams) {
            teamMap[team.teamId()] = team;
        }
        
        // 合并/更新网络数据
        int newTeamsCount = 0;
        int updatedTeamsCount = 0;
        for (const TeamData &networkTeam : teams) {
            QString teamId = networkTeam.teamId();
            if (teamMap.contains(teamId)) {
                // 更新现有队伍数据
                teamMap[teamId] = networkTeam;
                updatedTeamsCount++;
            } else {
                // 添加新队伍
                teamMap[teamId] = networkTeam;
                newTeamsCount++;
            }
        }
        
        // 更新队伍列表
        m_teams = teamMap.values();
        
        addAuditEntry(QString("混合模式数据合并完成：更新%1支队伍，新增%2支队伍")
                      .arg(updatedTeamsCount).arg(newTeamsCount));
    } else {
        // 在网络模式下，直接替换
        m_teams = teams;
        addAuditEntry(QString("网络数据接收完成，共%1支队伍").arg(teams.size()));
    }
    
    m_lastRefreshTime = QDateTime::currentDateTime();
    rebuildQueryTree();
    emit dataRefreshed();
    emit refreshFinished();
}

void DataManager::onNetworkError(const QString &error)
{
    addAuditEntry(QString("网络错误: %1").arg(error));
    
    if (m_dataSource == Hybrid) {
        fallbackToLocal();
    } else {
        emit errorOccurred(QString("网络错误: %1").arg(error));
        emit refreshFinished();
    }
    
    emit networkErrorOccurred(error);
}

void DataManager::onNetworkConnected()
{
    addAuditEntry("网络连接成功");
    emit networkConnected();
    
    if (m_dataSource == Network || m_dataSource == Hybrid) {
        m_networkManager->fetchAllTeams();
    }
}

void DataManager::onNetworkDisconnected()
{
    addAuditEntry("网络连接已断开");
    emit networkDisconnected();
}

bool DataManager::isNetworkConnected() const
{
    return m_networkManager->isConnected();
}

int DataManager::networkLatency() const
{
    return m_networkManager->networkLatency();
}
