#ifndef RANKINGMODEL_H
#define RANKINGMODEL_H

#include <QAbstractTableModel>
#include <QTimer>
#include "teamdata.h"

class RankingModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    enum Column {
        RankColumn = 0,
        TeamNameColumn,
        TotalScoreColumn,
        SolvedProblemsColumn,
        AccuracyColumn,
        LastSubmitTimeColumn,
        ColumnCount
    };

    enum SortType {
        SortByScore = 0,
        SortBySolved,
        SortByTime,
        SortByAccuracy
    };

    explicit RankingModel(QObject *parent = nullptr);

    // QAbstractTableModel接口
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    
    // 数据操作
    void setTeamData(const QList<TeamData> &teams);
    void addTeam(const TeamData &team);
    void updateTeam(const TeamData &team);
    void removeTeam(const QString &teamId);
    void clear();
    
    // 排序
    void setSortType(SortType type);
    SortType sortType() const { return m_sortType; }
    
    // 获取数据
    TeamData teamAt(int row) const;
    QList<TeamData> allTeams() const { return m_teams; }
    
    // 统计信息
    int totalTeams() const { return m_teams.size(); }
    QString topTeamName() const;

signals:
    void dataUpdated();

private slots:
    void sortData();

private:
    QList<TeamData> m_teams;
    SortType m_sortType;
    
    void calculateRanks();
    bool isTopThree(int rank) const;
};

#endif // RANKINGMODEL_H
