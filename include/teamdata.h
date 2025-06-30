#ifndef TEAMDATA_H
#define TEAMDATA_H

#include <QString>
#include <QDateTime>
#include <QJsonObject>
#include <QList>

struct Submission {
    QString problemId;
    QDateTime timestamp;
    bool isCorrect;
    int runTime;      // 运行时间(ms)
    int memoryUsage;  // 内存使用(bytes)
    
    Submission() : isCorrect(false), runTime(0), memoryUsage(0) {}
    
    QJsonObject toJson() const;
    void fromJson(const QJsonObject &json);
};

class TeamData
{
public:
    TeamData();
    TeamData(const QString &teamId, const QString &teamName);
    
    // 基本信息
    QString teamId() const { return m_teamId; }
    QString teamName() const { return m_teamName; }
    int totalScore() const { return m_totalScore; }
    QDateTime lastSubmitTime() const { return m_lastSubmitTime; }
    
    // 提交相关
    QList<Submission> submissions() const { return m_submissions; }
    void addSubmission(const Submission &submission);
    
    // 统计信息
    int solvedProblems() const;
    int totalSubmissions() const { return m_submissions.size(); }
    double accuracy() const;
    int averageTime() const;
    
    // 题目状态
    bool isProblemSolved(const QString &problemId) const;
    int problemScore(const QString &problemId) const;
    QDateTime problemSolveTime(const QString &problemId) const;
    
    // 序列化
    QJsonObject toJson() const;
    void fromJson(const QJsonObject &json);
    
    // 文件操作
    bool loadFromFile(const QString &filePath);
    bool saveToFile(const QString &filePath) const;
    
    // 校验
    QString calculateHash() const;
    bool verifyHash(const QString &hashFilePath) const;
    
private:
    QString m_teamId;
    QString m_teamName;
    QList<Submission> m_submissions;
    int m_totalScore;
    QDateTime m_lastSubmitTime;
    
    void updateStatistics();
};

#endif // TEAMDATA_H
