#ifndef DATAMANAGER_H
#define DATAMANAGER_H

#include <QObject>
#include <QTimer>
#include <QFileSystemWatcher>
#include <QDateTime>
#include "teamdata.h"
#include "binarysearchtree.h"

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

    // 新增的二叉树查询功能
    TeamQueryTree* getQueryTree() { return m_queryTree; }
    
    // 高级查询接口
    QList<TeamData> getTeamsSortedBy(TeamQueryTree::SortCriteria criteria);
    QList<TeamData> getTopTeamsByScore(int count);
    QList<TeamData> getBottomTeamsByScore(int count);
    QList<TeamData> getTeamsInScoreRange(int minScore, int maxScore);
    QList<TeamData> searchTeamsByName(const QString& namePattern);
    QList<TeamData> searchTeamsBySolvedProblems(int minSolved);
    QList<TeamData> searchTeamsByAccuracy(double minAccuracy);
    
    // 查询统计
    int getTeamRank(const QString& teamId) const;
    double getAverageScore() const;
    int getMedianScore() const;

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
    TeamQueryTree *m_queryTree;
    
    bool loadAllTeams();
    bool loadTeamFromFile(const QString &filePath);
    bool verifyFileIntegrity(const QString &jsonPath);
    QStringList findTeamFiles() const;
    void updateFileWatcher();
    void addAuditEntry(const QString &entry);
    void rebuildQueryTree();
    void updateQueryTree();
};

#endif // DATAMANAGER_H
