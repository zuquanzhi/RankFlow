#include "binarysearchtree.h"
#include <algorithm>
#include <QRegularExpression>

// TeamQueryTree 实现

TeamQueryTree::TeamQueryTree(QObject *parent)
    : QObject(parent), m_currentCriteria(ByTeamId)
{
}

TeamQueryTree::~TeamQueryTree()
{
}

void TeamQueryTree::buildTree(const QList<TeamData>& teams, SortCriteria criteria)
{
    // 暂时使用简单的实现，避免复杂的模板问题
    m_currentCriteria = criteria;
    m_teams = teams;
    
    // 简单排序而不是构建二叉树
    std::sort(m_teams.begin(), m_teams.end(), [criteria](const TeamData& a, const TeamData& b) {
        switch (criteria) {
            case ByTeamId:
                return a.teamId() < b.teamId();
            case ByTeamName:
                return a.teamName() < b.teamName();
            case ByTotalScore:
                return a.totalScore() > b.totalScore(); // 降序
            case ByLastSubmitTime:
                return a.lastSubmitTime() > b.lastSubmitTime();
            case BySolvedProblems:
                return a.solvedProblems() > b.solvedProblems();
            case ByAccuracy:
                return a.accuracy() > b.accuracy();
            default:
                return a.teamId() < b.teamId();
        }
    });
    
    emit treeRebuilt(criteria);
}

void TeamQueryTree::addTeam(const TeamData& team)
{
    m_teams.append(team);
    emit teamAdded(team.teamId());
}

void TeamQueryTree::removeTeam(const QString& teamId)
{
    // 从列表中找到并移除
    for (int i = 0; i < m_teams.size(); ++i) {
        if (m_teams[i].teamId() == teamId) {
            m_teams.removeAt(i);
            emit teamRemoved(teamId);
            break;
        }
    }
}

void TeamQueryTree::updateTeam(const TeamData& team)
{
    removeTeam(team.teamId());
    addTeam(team);
    emit teamUpdated(team.teamId());
}

void TeamQueryTree::clear()
{
    m_teams.clear();
}

QList<TeamData> TeamQueryTree::getAllTeams() const
{
    return m_teams; // 已经排序过的数据
}

QList<TeamData> TeamQueryTree::getTeamsInRange(const QString& minValue, const QString& maxValue) const
{
    if (m_teams.isEmpty()) {
        return QList<TeamData>();
    }
    
    // 简化的范围查询实现
    QList<TeamData> result;
    
    // 根据当前排序标准进行范围匹配
    switch (m_currentCriteria) {
        case ByTeamId:
            for (const TeamData& team : m_teams) {
                if (team.teamId() >= minValue && team.teamId() <= maxValue) {
                    result.append(team);
                }
            }
            break;
        case ByTeamName:
            for (const TeamData& team : m_teams) {
                if (team.teamName() >= minValue && team.teamName() <= maxValue) {
                    result.append(team);
                }
            }
            break;
        default:
            // 对于其他数值类型的标准，返回全部或根据字符串匹配
            result = m_teams;
            break;
    }
    
    return result;
}

QList<TeamData> TeamQueryTree::getTeamsInScoreRange(int minScore, int maxScore) const
{
    QList<TeamData> result;
    
    for (const TeamData& team : m_teams) {
        if (team.totalScore() >= minScore && team.totalScore() <= maxScore) {
            result.append(team);
        }
    }
    
    // 根据分数排序
    std::sort(result.begin(), result.end(), [](const TeamData& a, const TeamData& b) {
        return a.totalScore() > b.totalScore();
    });
    
    return result;
}

QList<TeamData> TeamQueryTree::getTopTeams(int count) const
{
    // 根据当前排序标准，如果是按分数排序则直接取前几个
    if (m_currentCriteria == ByTotalScore) {
        return m_teams.mid(0, qMin(count, m_teams.size()));
    } else {
        // 按分数重新排序
        QList<TeamData> sortedTeams = m_teams;
        std::sort(sortedTeams.begin(), sortedTeams.end(), [](const TeamData& a, const TeamData& b) {
            return a.totalScore() > b.totalScore();
        });
        return sortedTeams.mid(0, qMin(count, sortedTeams.size()));
    }
}

QList<TeamData> TeamQueryTree::getBottomTeams(int count) const
{
    // 根据当前排序标准
    if (m_currentCriteria == ByTotalScore) {
        // 如果已经按分数排序（降序），取后几个
        int startIndex = qMax(0, m_teams.size() - count);
        return m_teams.mid(startIndex);
    } else {
        // 按分数重新排序（升序）
        QList<TeamData> sortedTeams = m_teams;
        std::sort(sortedTeams.begin(), sortedTeams.end(), [](const TeamData& a, const TeamData& b) {
            return a.totalScore() < b.totalScore();
        });
        return sortedTeams.mid(0, qMin(count, sortedTeams.size()));
    }
}

TeamData TeamQueryTree::findTeam(const QString& teamId) const
{
    for (const TeamData& team : m_teams) {
        if (team.teamId() == teamId) {
            return team;
        }
    }
    return TeamData(); // 返回空的TeamData
}

QList<TeamData> TeamQueryTree::searchByName(const QString& namePattern) const
{
    QList<TeamData> result;
    
    for (const TeamData& team : m_teams) {
        if (matchesPattern(team.teamName(), namePattern)) {
            result.append(team);
        }
    }
    
    return result;
}

QList<TeamData> TeamQueryTree::searchBySolvedProblems(int minSolved) const
{
    QList<TeamData> result;
    
    for (const TeamData& team : m_teams) {
        if (team.solvedProblems() >= minSolved) {
            result.append(team);
        }
    }
    
    // 按解题数量排序
    std::sort(result.begin(), result.end(), [](const TeamData& a, const TeamData& b) {
        return a.solvedProblems() > b.solvedProblems();
    });
    
    return result;
}

QList<TeamData> TeamQueryTree::searchByAccuracy(double minAccuracy) const
{
    QList<TeamData> result;
    
    for (const TeamData& team : m_teams) {
        if (team.accuracy() >= minAccuracy) {
            result.append(team);
        }
    }
    
    // 按准确率排序
    std::sort(result.begin(), result.end(), [](const TeamData& a, const TeamData& b) {
        return a.accuracy() > b.accuracy();
    });
    
    return result;
}

int TeamQueryTree::totalTeams() const
{
    return m_teams.size();
}

bool TeamQueryTree::matchesPattern(const QString& text, const QString& pattern) const
{
    // 支持简单的通配符匹配
    QRegularExpression regex(QRegularExpression::wildcardToRegularExpression(pattern));
    return regex.match(text).hasMatch();
}
