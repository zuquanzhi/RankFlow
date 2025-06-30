#include <QCoreApplication>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QCryptographicHash>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    
    QString jsonPath = "/Users/zuquanzhi/Programs/rank/data/team01_results.json";
    QString hashPath = jsonPath + ".sha256";
    
    // 读取JSON文件
    QFile file(jsonPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "无法打开JSON文件:" << jsonPath;
        return 1;
    }
    
    QByteArray data = file.readAll();
    file.close();
    
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    if (error.error != QJsonParseError::NoError) {
        qDebug() << "JSON解析错误:" << error.errorString();
        return 1;
    }
    
    // 读取存储的哈希
    QFile hashFile(hashPath);
    if (!hashFile.open(QIODevice::ReadOnly)) {
        qDebug() << "无法打开哈希文件:" << hashPath;
        return 1;
    }
    QString storedHash = QString::fromUtf8(hashFile.readAll()).trimmed();
    hashFile.close();
    
    // 计算紧凑格式的哈希
    QByteArray compactData = doc.toJson(QJsonDocument::Compact);
    QString calculatedHash = QString(QCryptographicHash::hash(compactData, QCryptographicHash::Sha256).toHex());
    
    qDebug() << "JSON文件:" << jsonPath;
    qDebug() << "存储哈希:" << storedHash;
    qDebug() << "计算哈希:" << calculatedHash;
    qDebug() << "匹配:" << (storedHash == calculatedHash);
    qDebug() << "紧凑JSON长度:" << compactData.size();
    qDebug() << "紧凑JSON前100字符:" << QString::fromUtf8(compactData.left(100));
    
    return 0;
}
