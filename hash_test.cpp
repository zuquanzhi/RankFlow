#include <QCoreApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFile>
#include <QCryptographicHash>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    QString filePath = "/Users/zuquanzhi/Programs/rank/data/test_team_alpha.json";
    QString hashPath = "/Users/zuquanzhi/Programs/rank/data/test_team_alpha.json.sha256";
    
    // 读取文件内容
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "无法打开文件:" << filePath;
        return 1;
    }
    
    QByteArray fileData = file.readAll();
    file.close();
    
    // 解析JSON
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(fileData, &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug() << "JSON解析错误:" << error.errorString();
        return 1;
    }
    
    // 计算原文件的哈希（格式化JSON）
    QString originalHash = QString(QCryptographicHash::hash(fileData, QCryptographicHash::Sha256).toHex());
    qDebug() << "原文件哈希（格式化JSON）:" << originalHash;
    
    // 计算紧凑格式的哈希
    QByteArray compactData = doc.toJson(QJsonDocument::Compact);
    QString compactHash = QString(QCryptographicHash::hash(compactData, QCryptographicHash::Sha256).toHex());
    qDebug() << "紧凑格式哈希:" << compactHash;
    
    // 读取存储的哈希
    QFile hashFile(hashPath);
    if (hashFile.open(QIODevice::ReadOnly)) {
        QString storedHash = QString::fromUtf8(hashFile.readAll()).trimmed();
        qDebug() << "存储的哈希:" << storedHash;
        qDebug() << "原文件哈希匹配:" << (originalHash == storedHash);
        qDebug() << "紧凑格式匹配:" << (compactHash == storedHash);
        hashFile.close();
    }
    
    return 0;
}
