#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <QTimer>
#include <QFileSystemWatcher>
#include <QDateTime>
#include "teamdata.h"

class DataManager : public QObject
{
    Q_OBJECT

public:
    explicit DataManager(QObject *parent = nullptr);
    
    // 配置
    void setDataDirectory(const QString &path);
    void setRefreshInterval(int seconds);
    void setAutoRefresh(bool enabled);
    
    // 手动操作
    void refreshData();
    bool loadTeamData(const QString &teamId);
    
    // 获取数据
    QList<TeamData> allTeams() const { return m_teams; }
    TeamData getTeam(const QString &teamId) const;
    QDateTime lastRefreshTime() const { return m_lastRefreshTime; }
    
    // 统计信息
    int totalTeams() const { return m_teams.size(); }
    QStringList availableProblems() const;
    
    // 审计日志
    void logOperation(const QString &operation);
    QStringList getAuditLog() const;

signals:
    void dataRefreshed();
    void teamDataChanged(const QString &teamId);
    void errorOccurred(const QString &error);
    void refreshStarted();
    void refreshFinished();

public slots:
    void startAutoRefresh();
    void stopAutoRefresh();

private slots:
    void onRefreshTimer();
    void onFileChanged(const QString &path);

private:
    QString m_dataDirectory;
    QList<TeamData> m_teams;
    QTimer *m_refreshTimer;
    QFileSystemWatcher *m_fileWatcher;
    QDateTime m_lastRefreshTime;
    QStringList m_auditLog;
    
    bool loadAllTeams();
    bool loadTeamFromFile(const QString &filePath);
    bool verifyFileIntegrity(const QString &jsonPath);
    QStringList findTeamFiles() const;
    void updateFileWatcher();
    void addAuditEntry(const QString &entry);
};

#endif // DATAMANAGER_H
