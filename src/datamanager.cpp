#include "datamanager.h"
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QStandardPaths>

DataManager::DataManager(QObject *parent)
    : QObject(parent)
    , m_refreshTimer(new QTimer(this))
    , m_fileWatcher(new QFileSystemWatcher(this))
{
    // 默认数据目录
    m_dataDirectory = "data";
    
    // 设置定时器
    m_refreshTimer->setSingleShot(false);
    connect(m_refreshTimer, &QTimer::timeout, this, &DataManager::onRefreshTimer);
    
    // 设置文件监视器
    connect(m_fileWatcher, &QFileSystemWatcher::fileChanged, 
            this, &DataManager::onFileChanged);
    
    // 默认刷新间隔10分钟
    setRefreshInterval(600);
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
    addAuditEntry("开始手动刷新数据");
    
    if (loadAllTeams()) {
        m_lastRefreshTime = QDateTime::currentDateTime();
        emit dataRefreshed();
        addAuditEntry(QString("数据刷新完成，共载入%1支队伍").arg(m_teams.size()));
    } else {
        emit errorOccurred("数据刷新失败");
        addAuditEntry("数据刷新失败");
    }
    
    emit refreshFinished();
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
    QString hashPath = jsonPath;
    hashPath.replace(".json", ".sha256");
    
    QFileInfo hashFile(hashPath);
    if (!hashFile.exists()) {
        // 如果没有校验文件，暂时跳过验证
        return true;
    }
    
    TeamData team;
    if (!team.loadFromFile(jsonPath)) {
        return false;
    }
    
    return team.verifyHash(hashPath);
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
