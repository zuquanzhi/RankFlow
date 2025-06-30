#include "teamdata.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>
#include <QCryptographicHash>
#include <QDebug>

QJsonObject Submission::toJson() const
{
    QJsonObject obj;
    obj["problem_id"] = problemId;
    obj["timestamp"] = timestamp.toString(Qt::ISODate);
    obj["is_correct"] = isCorrect;
    obj["run_time"] = runTime;
    obj["memory_usage"] = memoryUsage;
    return obj;
}

void Submission::fromJson(const QJsonObject &json)
{
    problemId = json["problem_id"].toString();
    timestamp = QDateTime::fromString(json["timestamp"].toString(), Qt::ISODate);
    isCorrect = json["is_correct"].toBool();
    runTime = json["run_time"].toInt();
    memoryUsage = json["memory_usage"].toInt();
}

TeamData::TeamData()
    : m_totalScore(0)
{
}

TeamData::TeamData(const QString &teamId, const QString &teamName)
    : m_teamId(teamId), m_teamName(teamName), m_totalScore(0)
{
}

void TeamData::addSubmission(const Submission &submission)
{
    m_submissions.append(submission);
    updateStatistics();
}

int TeamData::solvedProblems() const
{
    QStringList solvedList;
    for (const auto &submission : m_submissions) {
        if (submission.isCorrect && !solvedList.contains(submission.problemId)) {
            solvedList.append(submission.problemId);
        }
    }
    return solvedList.size();
}

double TeamData::accuracy() const
{
    if (m_submissions.isEmpty()) return 0.0;
    
    int correctCount = 0;
    for (const auto &submission : m_submissions) {
        if (submission.isCorrect) correctCount++;
    }
    
    return static_cast<double>(correctCount) / m_submissions.size() * 100.0;
}

int TeamData::averageTime() const
{
    if (m_submissions.isEmpty()) return 0;
    
    int totalTime = 0;
    for (const auto &submission : m_submissions) {
        totalTime += submission.runTime;
    }
    
    return totalTime / m_submissions.size();
}

bool TeamData::isProblemSolved(const QString &problemId) const
{
    for (const auto &submission : m_submissions) {
        if (submission.problemId == problemId && submission.isCorrect) {
            return true;
        }
    }
    return false;
}

int TeamData::problemScore(const QString &problemId) const
{
    if (isProblemSolved(problemId)) {
        return 100; // 基础分数
    }
    return 0;
}

QDateTime TeamData::problemSolveTime(const QString &problemId) const
{
    for (const auto &submission : m_submissions) {
        if (submission.problemId == problemId && submission.isCorrect) {
            return submission.timestamp;
        }
    }
    return QDateTime();
}

QJsonObject TeamData::toJson() const
{
    QJsonObject obj;
    obj["team_id"] = m_teamId;
    obj["team_name"] = m_teamName;
    obj["total_score"] = m_totalScore;
    obj["last_submit_time"] = m_lastSubmitTime.toString(Qt::ISODate);
    
    QJsonArray submissionsArray;
    for (const auto &submission : m_submissions) {
        submissionsArray.append(submission.toJson());
    }
    obj["submissions"] = submissionsArray;
    
    return obj;
}

void TeamData::fromJson(const QJsonObject &json)
{
    m_teamId = json["team_id"].toString();
    m_teamName = json["team_name"].toString();
    m_totalScore = json["total_score"].toInt();
    m_lastSubmitTime = QDateTime::fromString(json["last_submit_time"].toString(), Qt::ISODate);
    
    m_submissions.clear();
    QJsonArray submissionsArray = json["submissions"].toArray();
    for (const auto &value : submissionsArray) {
        Submission submission;
        submission.fromJson(value.toObject());
        m_submissions.append(submission);
    }
}

bool TeamData::loadFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "无法打开文件:" << filePath;
        return false;
    }
    
    QByteArray data = file.readAll();
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    
    if (error.error != QJsonParseError::NoError) {
        qDebug() << "JSON解析错误:" << error.errorString();
        return false;
    }
    
    fromJson(doc.object());
    updateStatistics();
    return true;
}

bool TeamData::saveToFile(const QString &filePath) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }
    
    QJsonDocument doc(toJson());
    file.write(doc.toJson());
    return true;
}

QString TeamData::calculateHash() const
{
    QJsonDocument doc(toJson());
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    return QString(QCryptographicHash::hash(data, QCryptographicHash::Sha256).toHex());
}

bool TeamData::verifyHash(const QString &hashFilePath) const
{
    QFile file(hashFilePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }
    
    QString storedHash = QString::fromUtf8(file.readAll()).trimmed();
    QString calculatedHash = calculateHash();
    
    return storedHash == calculatedHash;
}

void TeamData::updateStatistics()
{
    m_totalScore = solvedProblems() * 100; // 每题100分
    
    if (!m_submissions.isEmpty()) {
        m_lastSubmitTime = m_submissions.last().timestamp;
    }
}
