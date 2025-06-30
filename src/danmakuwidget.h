#ifndef DANMAKUWIDGET_H
#define DANMAKUWIDGET_H

#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QTimer>
#include <QScrollBar>
#include <QComboBox>

class DanmakuWidget : public QWidget
{
    Q_OBJECT

public:
    explicit DanmakuWidget(QWidget *parent = nullptr);
    
    void addMessage(const QString &user, const QString &message);
    void addSystemMessage(const QString &message);

public slots:
    void onSendMessage();
    void onClearMessages();
    void enableFilter(bool enabled);

private slots:
    void onAutoScrollToggled(bool enabled);
    void generateRandomMessages();

private:
    void setupUI();
    void setupRandomMessages();
    QString filterMessage(const QString &message);
    
    QTextEdit *m_messageDisplay;
    QLineEdit *m_messageInput;
    QPushButton *m_sendButton;
    QPushButton *m_clearButton;
    QComboBox *m_userCombo;
    QPushButton *m_autoScrollButton;
    QTimer *m_randomMessageTimer;
    
    bool m_autoScroll;
    bool m_filterEnabled;
    QStringList m_encourageMessages;
    QStringList m_userNames;
    
    int m_messageCount;
};

#endif // DANMAKUWIDGET_H
