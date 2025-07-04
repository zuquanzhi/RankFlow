#include "danmakuwidget.h"
#include <QDateTime>
#include <QScrollBar>
#include <QRandomGenerator>
#include <QRegularExpression>

DanmakuWidget::DanmakuWidget(QWidget *parent)
    : QWidget(parent)
    , m_autoScroll(true)
    , m_filterEnabled(true)
    , m_messageCount(0)
{
    setupUI();
    setupRandomMessages();
    
    // 启动随机消息定时器
    m_randomMessageTimer = new QTimer(this);
    connect(m_randomMessageTimer, &QTimer::timeout, this, &DanmakuWidget::generateRandomMessages);
    m_randomMessageTimer->start(30000); // 每30秒生成一条随机消息
}

void DanmakuWidget::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout;
    
    // 标题和控制按钮
    QHBoxLayout *titleLayout = new QHBoxLayout;
    QLabel *titleLabel = new QLabel("观众互动区");
    titleLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    
    m_autoScrollButton = new QPushButton("自动滚动");
    m_autoScrollButton->setCheckable(true);
    m_autoScrollButton->setChecked(true);
    m_autoScrollButton->setMaximumWidth(80);
    
    m_clearButton = new QPushButton("清空");
    m_clearButton->setMaximumWidth(60);
    
    titleLayout->addWidget(titleLabel);
    titleLayout->addStretch();
    titleLayout->addWidget(m_autoScrollButton);
    titleLayout->addWidget(m_clearButton);
    
    // 消息显示区域
    m_messageDisplay = new QTextEdit;
    m_messageDisplay->setReadOnly(true);
    m_messageDisplay->setMaximumHeight(200);
    m_messageDisplay->setStyleSheet(
        "QTextEdit {"
        "    background-color: #2c3e50;"
        "    border: 1px solid #34495e;"
        "    color: #ecf0f1;"
        "    font-family: 'Microsoft YaHei', sans-serif;"
        "    font-size: 12px;"
        "}"
    );
    
    // 输入区域
    QHBoxLayout *inputLayout = new QHBoxLayout;
    
    m_userCombo = new QComboBox;
    m_userCombo->setEditable(true);
    m_userCombo->setMaximumWidth(100);
    m_userCombo->addItems(QStringList() << "观众" << "粉丝" << "支持者" << "加油团");
    
    m_messageInput = new QLineEdit;
    m_messageInput->setPlaceholderText("为参赛队伍加油...");
    m_messageInput->setMaxLength(100);
    
    m_sendButton = new QPushButton("发送");
    m_sendButton->setMaximumWidth(60);
    
    inputLayout->addWidget(new QLabel("用户:"));
    inputLayout->addWidget(m_userCombo);
    inputLayout->addWidget(m_messageInput);
    inputLayout->addWidget(m_sendButton);
    
    layout->addLayout(titleLayout);
    layout->addWidget(m_messageDisplay);
    layout->addLayout(inputLayout);
    setLayout(layout);
    
    // 连接信号
    connect(m_sendButton, &QPushButton::clicked, this, &DanmakuWidget::onSendMessage);
    connect(m_clearButton, &QPushButton::clicked, this, &DanmakuWidget::onClearMessages);
    connect(m_autoScrollButton, &QPushButton::toggled, this, &DanmakuWidget::onAutoScrollToggled);
    connect(m_messageInput, &QLineEdit::returnPressed, this, &DanmakuWidget::onSendMessage);
    
    // 添加欢迎消息
    addSystemMessage("欢迎来到竞赛排行榜系统！为参赛队伍加油吧！");
}

void DanmakuWidget::setupRandomMessages()
{
    QStringList encourageMessages;
    encourageMessages << "加油！冲冲冲！"
                     << "太厉害了！"
                     << "这道题有点难啊"
                     << "排名变化好激烈"
                     << "支持第一名！"
                     << "期待反超！"
                     << "代码写得真快"
                     << "算法太强了"
                     << "这个效率！"
                     << "紧张刺激！"
                     << "实力相当啊"
                     << "高手如云"
                     << "精彩的比赛"
                     << "技术流！"
                     << "思路清晰"
                     << "速度惊人"
                     << "逻辑严密"
                     << "创意十足"
                     << "完美解答"
                     << "天秀操作"
                     << "原神启动";
    m_encourageMessages = encourageMessages;
    
    QStringList userNames;
    userNames << "strong哥"
              << "诗人我吃"
              << "代码狂人"
              << "root"
              << "开发者"
              << "程序猿"
              << "码农"
              << "deepseek"
              << "OpenAI"
              << "代码诗人";
    m_userNames = userNames;
}

void DanmakuWidget::addMessage(const QString &user, const QString &message)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString filteredMessage = m_filterEnabled ? filterMessage(message) : message;
    
    QString html = QString("<span style='color: #95a5a6;'>[%1]</span> "
                          "<span style='color: #3498db; font-weight: bold;'>%2:</span> "
                          "<span style='color: #ecf0f1;'>%3</span>")
                   .arg(timestamp, user, filteredMessage);
    
    m_messageDisplay->append(html);
    m_messageCount++;
    
    // 限制消息数量，避免内存过度使用
    if (m_messageCount > 100) {
        QTextCursor cursor = m_messageDisplay->textCursor();
        cursor.movePosition(QTextCursor::Start);
        cursor.movePosition(QTextCursor::Down, QTextCursor::KeepAnchor, 1);
        cursor.removeSelectedText();
        m_messageCount--;
    }
    
    // 自动滚动到底部
    if (m_autoScroll) {
        QScrollBar *scrollBar = m_messageDisplay->verticalScrollBar();
        scrollBar->setValue(scrollBar->maximum());
    }
}

void DanmakuWidget::addSystemMessage(const QString &message)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    
    QString html = QString("<span style='color: #95a5a6;'>[%1]</span> "
                          "<span style='color: #e74c3c; font-weight: bold;'>[系统]:</span> "
                          "<span style='color: #f39c12;'>%2</span>")
                   .arg(timestamp, message);
    
    m_messageDisplay->append(html);
    m_messageCount++;
    
    if (m_autoScroll) {
        QScrollBar *scrollBar = m_messageDisplay->verticalScrollBar();
        scrollBar->setValue(scrollBar->maximum());
    }
}

void DanmakuWidget::onSendMessage()
{
    QString message = m_messageInput->text().trimmed();
    if (message.isEmpty()) {
        return;
    }
    
    QString user = m_userCombo->currentText();
    if (user.isEmpty()) {
        user = "匿名用户";
    }
    
    addMessage(user, message);
    m_messageInput->clear();
}

void DanmakuWidget::onClearMessages()
{
    m_messageDisplay->clear();
    m_messageCount = 0;
    addSystemMessage("消息已清空");
}

void DanmakuWidget::enableFilter(bool enabled)
{
    m_filterEnabled = enabled;
}

void DanmakuWidget::onAutoScrollToggled(bool enabled)
{
    m_autoScroll = enabled;
    
    if (enabled) {
        m_autoScrollButton->setText("自动滚动");
        m_autoScrollButton->setStyleSheet("");
    } else {
        m_autoScrollButton->setText("手动滚动");
        m_autoScrollButton->setStyleSheet("background-color: #e74c3c; color: white;");
    }
}

void DanmakuWidget::generateRandomMessages()
{
    // 随机生成观众互动消息
    if (QRandomGenerator::global()->bounded(100) < 30) { // 30%概率生成消息
        QString user = m_userNames[QRandomGenerator::global()->bounded(m_userNames.size())];
        QString message = m_encourageMessages[QRandomGenerator::global()->bounded(m_encourageMessages.size())];
        
        addMessage(user, message);
    }
}

QString DanmakuWidget::filterMessage(const QString &message)
{
    QString filtered = message;
    
    // 简单的敏感词过滤
    QStringList bannedWords = {"垃圾", "傻逼", "白痴", "废物"};
    
    for (const QString &word : bannedWords) {
        if (filtered.contains(word, Qt::CaseInsensitive)) {
            QString replacement = QString("*").repeated(word.length());
            filtered.replace(word, replacement, Qt::CaseInsensitive);
        }
    }
    
    return filtered;
}
