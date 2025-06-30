#include "rankingmodel.h"
#include <QColor>
#include <QFont>
#include <algorithm>

RankingModel::RankingModel(QObject *parent)
    : QAbstractTableModel(parent), m_sortType(SortByScore)
{
}

int RankingModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_teams.size();
}

int RankingModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return ColumnCount;
}

QVariant RankingModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_teams.size())
        return QVariant();

    const TeamData &team = m_teams.at(index.row());
    int rank = index.row() + 1;

    switch (role) {
    case Qt::DisplayRole:
        switch (index.column()) {
        case RankColumn:
            return rank;
        case TeamNameColumn:
            return team.teamName();
        case TotalScoreColumn:
            return team.totalScore();
        case SolvedProblemsColumn:
            return team.solvedProblems();
        case AccuracyColumn:
            return QString::number(team.accuracy(), 'f', 1) + "%";
        case LastSubmitTimeColumn:
            return team.lastSubmitTime().toString("hh:mm:ss");
        default:
            return QVariant();
        }

    case Qt::BackgroundRole:
        if (isTopThree(rank)) {
            switch (rank) {
            case 1: return QColor(255, 215, 0, 100);  // 金色
            case 2: return QColor(192, 192, 192, 100); // 银色
            case 3: return QColor(205, 127, 50, 100);  // 铜色
            }
        }
        return QVariant();

    case Qt::ForegroundRole:
        if (isTopThree(rank)) {
            return QColor(Qt::white);
        }
        return QVariant();

    case Qt::FontRole:
        if (isTopThree(rank)) {
            QFont font;
            font.setBold(true);
            return font;
        }
        return QVariant();

    case Qt::TextAlignmentRole:
        if (index.column() == RankColumn || 
            index.column() == TotalScoreColumn || 
            index.column() == SolvedProblemsColumn ||
            index.column() == AccuracyColumn) {
            return Qt::AlignCenter;
        }
        return QVariant();

    default:
        return QVariant();
    }
}

QVariant RankingModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole)
        return QVariant();

    switch (section) {
    case RankColumn:
        return "排名";
    case TeamNameColumn:
        return "队伍名称";
    case TotalScoreColumn:
        return "总分";
    case SolvedProblemsColumn:
        return "通过题数";
    case AccuracyColumn:
        return "准确率";
    case LastSubmitTimeColumn:
        return "最后提交";
    default:
        return QVariant();
    }
}

void RankingModel::setTeamData(const QList<TeamData> &teams)
{
    beginResetModel();
    m_teams = teams;
    sortData();
    endResetModel();
    emit dataUpdated();
}

void RankingModel::addTeam(const TeamData &team)
{
    // 检查是否已存在
    for (int i = 0; i < m_teams.size(); ++i) {
        if (m_teams[i].teamId() == team.teamId()) {
            m_teams[i] = team;
            sortData();
            emit dataChanged(createIndex(0, 0), createIndex(m_teams.size() - 1, ColumnCount - 1));
            emit dataUpdated();
            return;
        }
    }

    beginInsertRows(QModelIndex(), m_teams.size(), m_teams.size());
    m_teams.append(team);
    endInsertRows();
    
    sortData();
    emit dataUpdated();
}

void RankingModel::updateTeam(const TeamData &team)
{
    for (int i = 0; i < m_teams.size(); ++i) {
        if (m_teams[i].teamId() == team.teamId()) {
            m_teams[i] = team;
            sortData();
            emit dataChanged(createIndex(0, 0), createIndex(m_teams.size() - 1, ColumnCount - 1));
            emit dataUpdated();
            return;
        }
    }
}

void RankingModel::removeTeam(const QString &teamId)
{
    for (int i = 0; i < m_teams.size(); ++i) {
        if (m_teams[i].teamId() == teamId) {
            beginRemoveRows(QModelIndex(), i, i);
            m_teams.removeAt(i);
            endRemoveRows();
            emit dataUpdated();
            return;
        }
    }
}

void RankingModel::clear()
{
    beginResetModel();
    m_teams.clear();
    endResetModel();
    emit dataUpdated();
}

void RankingModel::setSortType(SortType type)
{
    if (m_sortType != type) {
        m_sortType = type;
        sortData();
    }
}

TeamData RankingModel::teamAt(int row) const
{
    if (row >= 0 && row < m_teams.size()) {
        return m_teams.at(row);
    }
    return TeamData();
}

QString RankingModel::topTeamName() const
{
    if (!m_teams.isEmpty()) {
        return m_teams.first().teamName();
    }
    return QString();
}

void RankingModel::sortData()
{
    beginResetModel();
    
    std::sort(m_teams.begin(), m_teams.end(), [this](const TeamData &a, const TeamData &b) {
        switch (m_sortType) {
        case SortByScore:
            if (a.totalScore() != b.totalScore()) {
                return a.totalScore() > b.totalScore();
            }
            // 分数相同时按通过题数排序
            if (a.solvedProblems() != b.solvedProblems()) {
                return a.solvedProblems() > b.solvedProblems();
            }
            // 通过题数相同时按最后提交时间排序（越早越好）
            return a.lastSubmitTime() < b.lastSubmitTime();
            
        case SortBySolved:
            if (a.solvedProblems() != b.solvedProblems()) {
                return a.solvedProblems() > b.solvedProblems();
            }
            return a.totalScore() > b.totalScore();
            
        case SortByTime:
            return a.lastSubmitTime() < b.lastSubmitTime();
            
        case SortByAccuracy:
            if (qAbs(a.accuracy() - b.accuracy()) > 0.01) {
                return a.accuracy() > b.accuracy();
            }
            return a.totalScore() > b.totalScore();
            
        default:
            return a.totalScore() > b.totalScore();
        }
    });
    
    calculateRanks();
    endResetModel();
}

void RankingModel::calculateRanks()
{
    // 排名已通过排序确定，这里可以处理并列排名的逻辑
    // 当前实现为简单排序，如需并列排名可在此扩展
}

bool RankingModel::isTopThree(int rank) const
{
    return rank >= 1 && rank <= 3;
}
