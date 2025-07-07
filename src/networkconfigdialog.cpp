#include "networkconfigdialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QProgressBar>
#include <QMessageBox>
#include <QSettings>

NetworkConfigDialog::NetworkConfigDialog(DataManager *dataManager, QWidget *parent)
    : QDialog(parent)
    , m_dataManager(dataManager)
{
    setWindowTitle("网络配置");
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setMinimumSize(500, 600);
    
    setupUI();
    loadCurrentSettings();
    
    // 连接DataManager的网络信号
    connect(m_dataManager, &DataManager::networkConnected,
            this, &NetworkConfigDialog::updateConnectionStatus);
    connect(m_dataManager, &DataManager::networkDisconnected,
            this, &NetworkConfigDialog::updateConnectionStatus);
    connect(m_dataManager, &DataManager::networkErrorOccurred,
            this, &NetworkConfigDialog::onNetworkError);
}

void NetworkConfigDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    // 服务器配置组
    m_serverGroup = new QGroupBox("服务器配置");
    QFormLayout *serverLayout = new QFormLayout(m_serverGroup);
    
    m_serverUrlEdit = new QLineEdit;
    m_serverUrlEdit->setPlaceholderText("http://example.com");
    serverLayout->addRow("服务器地址:", m_serverUrlEdit);
    
    m_serverPortSpin = new QSpinBox;
    m_serverPortSpin->setRange(1, 65535);
    m_serverPortSpin->setValue(80);
    serverLayout->addRow("端口:", m_serverPortSpin);
    
    m_apiEndpointEdit = new QLineEdit;
    m_apiEndpointEdit->setPlaceholderText("/api/teams");
    serverLayout->addRow("API端点:", m_apiEndpointEdit);
    
    m_dataSourceCombo = new QComboBox;
    m_dataSourceCombo->addItem("HTTP REST API", static_cast<int>(NetworkManager::HTTP_API));
    m_dataSourceCombo->addItem("DOMjudge API", static_cast<int>(NetworkManager::DOMJUDGE_API));
    m_dataSourceCombo->addItem("ICPC Tools", static_cast<int>(NetworkManager::ICPC_TOOLS));
    serverLayout->addRow("数据源类型:", m_dataSourceCombo);
    
    mainLayout->addWidget(m_serverGroup);
    
    // 认证配置组
    m_authGroup = new QGroupBox("认证配置");
    QFormLayout *authLayout = new QFormLayout(m_authGroup);
    
    m_authTypeCombo = new QComboBox;
    m_authTypeCombo->addItem("无认证", 0);
    m_authTypeCombo->addItem("基本认证", 1);
    m_authTypeCombo->addItem("API密钥", 2);
    authLayout->addRow("认证方式:", m_authTypeCombo);
    
    m_usernameEdit = new QLineEdit;
    authLayout->addRow("用户名:", m_usernameEdit);
    
    m_passwordEdit = new QLineEdit;
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    authLayout->addRow("密码:", m_passwordEdit);
    
    m_apiKeyEdit = new QLineEdit;
    m_apiKeyEdit->setEchoMode(QLineEdit::Password);
    authLayout->addRow("API密钥:", m_apiKeyEdit);
    
    mainLayout->addWidget(m_authGroup);
    
    // 连接配置组
    m_connectionGroup = new QGroupBox("连接配置");
    QFormLayout *connectionLayout = new QFormLayout(m_connectionGroup);
    
    m_updateIntervalSpin = new QSpinBox;
    m_updateIntervalSpin->setRange(1000, 300000);
    m_updateIntervalSpin->setValue(5000);
    m_updateIntervalSpin->setSuffix(" ms");
    connectionLayout->addRow("更新间隔:", m_updateIntervalSpin);
    
    m_autoReconnectCheck = new QCheckBox("启用自动重连");
    m_autoReconnectCheck->setChecked(true);
    connectionLayout->addRow(m_autoReconnectCheck);
    
    m_maxRetriesSpin = new QSpinBox;
    m_maxRetriesSpin->setRange(0, 10);
    m_maxRetriesSpin->setValue(3);
    connectionLayout->addRow("最大重试次数:", m_maxRetriesSpin);
    
    m_retryIntervalSpin = new QSpinBox;
    m_retryIntervalSpin->setRange(1, 60);
    m_retryIntervalSpin->setValue(5);
    m_retryIntervalSpin->setSuffix(" 秒");
    connectionLayout->addRow("重试间隔:", m_retryIntervalSpin);
    
    m_sslVerifyCheck = new QCheckBox("验证SSL证书");
    m_sslVerifyCheck->setChecked(true);
    connectionLayout->addRow(m_sslVerifyCheck);
    
    mainLayout->addWidget(m_connectionGroup);
    
    // 状态信息组
    QGroupBox *statusGroup = new QGroupBox("连接状态");
    QFormLayout *statusLayout = new QFormLayout(statusGroup);
    
    m_statusLabel = new QLabel("未连接");
    statusLayout->addRow("状态:", m_statusLabel);
    
    m_latencyLabel = new QLabel("未知");
    statusLayout->addRow("延迟:", m_latencyLabel);
    
    m_lastUpdateLabel = new QLabel("未知");
    statusLayout->addRow("最后更新:", m_lastUpdateLabel);
    
    m_bytesLabel = new QLabel("0 字节");
    statusLayout->addRow("已接收:", m_bytesLabel);
    
    m_progressBar = new QProgressBar;
    m_progressBar->setVisible(false);
    statusLayout->addRow("进度:", m_progressBar);
    
    mainLayout->addWidget(statusGroup);
    
    // 按钮区域
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    
    m_testButton = new QPushButton("测试连接");
    m_testButton->setIcon(style()->standardIcon(QStyle::SP_DialogApplyButton));
    connect(m_testButton, &QPushButton::clicked, this, &NetworkConfigDialog::onTestConnection);
    
    m_applyButton = new QPushButton("应用");
    m_applyButton->setIcon(style()->standardIcon(QStyle::SP_DialogOkButton));
    connect(m_applyButton, &QPushButton::clicked, this, &NetworkConfigDialog::onApplySettings);
    
    m_resetButton = new QPushButton("重置");
    m_resetButton->setIcon(style()->standardIcon(QStyle::SP_DialogResetButton));
    connect(m_resetButton, &QPushButton::clicked, this, &NetworkConfigDialog::onResetToDefaults);
    
    m_closeButton = new QPushButton("关闭");
    m_closeButton->setIcon(style()->standardIcon(QStyle::SP_DialogCloseButton));
    connect(m_closeButton, &QPushButton::clicked, this, &QDialog::accept);
    
    buttonLayout->addWidget(m_testButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_resetButton);
    buttonLayout->addWidget(m_applyButton);
    buttonLayout->addWidget(m_closeButton);
    
    mainLayout->addLayout(buttonLayout);
    
    // 连接信号
    connect(m_authTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &NetworkConfigDialog::onDataSourceChanged);
    
    onDataSourceChanged(); // 初始化UI状态
}

void NetworkConfigDialog::loadCurrentSettings()
{
    QSettings settings;
    
    // 服务器配置
    m_serverUrlEdit->setText(settings.value("network/serverUrl", "http://localhost:8080").toString());
    m_serverPortSpin->setValue(settings.value("network/serverPort", 8080).toInt());
    m_apiEndpointEdit->setText(settings.value("network/apiEndpoint", "/api/teams").toString());
    
    // 认证配置
    m_authTypeCombo->setCurrentIndex(settings.value("network/authType", 0).toInt());
    m_usernameEdit->setText(settings.value("network/username").toString());
    m_apiKeyEdit->setText(settings.value("network/apiKey").toString());
    
    // 连接配置
    m_updateIntervalSpin->setValue(settings.value("network/updateInterval", 5000).toInt());
    m_autoReconnectCheck->setChecked(settings.value("network/autoReconnect", true).toBool());
    m_maxRetriesSpin->setValue(settings.value("network/maxRetries", 3).toInt());
    m_retryIntervalSpin->setValue(settings.value("network/retryInterval", 5).toInt());
    m_sslVerifyCheck->setChecked(settings.value("network/sslVerify", true).toBool());
}

void NetworkConfigDialog::saveSettings()
{
    QSettings settings;
    
    // 服务器配置
    settings.setValue("network/serverUrl", m_serverUrlEdit->text());
    settings.setValue("network/serverPort", m_serverPortSpin->value());
    settings.setValue("network/apiEndpoint", m_apiEndpointEdit->text());
    settings.setValue("network/dataSource", m_dataSourceCombo->currentData().toInt());
    
    // 认证配置
    settings.setValue("network/authType", m_authTypeCombo->currentIndex());
    settings.setValue("network/username", m_usernameEdit->text());
    settings.setValue("network/apiKey", m_apiKeyEdit->text());
    
    // 连接配置
    settings.setValue("network/updateInterval", m_updateIntervalSpin->value());
    settings.setValue("network/autoReconnect", m_autoReconnectCheck->isChecked());
    settings.setValue("network/maxRetries", m_maxRetriesSpin->value());
    settings.setValue("network/retryInterval", m_retryIntervalSpin->value());
    settings.setValue("network/sslVerify", m_sslVerifyCheck->isChecked());
}

void NetworkConfigDialog::onTestConnection()
{
    // 应用当前设置
    onApplySettings();
    
    // 显示进度条
    m_progressBar->setVisible(true);
    m_progressBar->setRange(0, 0);
    m_testButton->setEnabled(false);
    m_statusLabel->setText("测试中...");
    m_statusLabel->setStyleSheet("color: #f39c12;");
    
    // 启用网络并尝试连接
    m_dataManager->setNetworkEnabled(true);
}

void NetworkConfigDialog::onApplySettings()
{
    // 配置服务器
    QString serverUrl = m_serverUrlEdit->text();
    if (m_serverPortSpin->value() != 80 && m_serverPortSpin->value() != 443) {
        serverUrl += ":" + QString::number(m_serverPortSpin->value());
    }
    
    m_dataManager->setServerUrl(serverUrl);
    m_dataManager->setApiEndpoint(m_apiEndpointEdit->text());
    
    // 配置认证
    int authType = m_authTypeCombo->currentIndex();
    if (authType == 1) { // 基本认证
        m_dataManager->setApiAuthentication(m_usernameEdit->text(), m_passwordEdit->text());
    } else if (authType == 2) { // API密钥
        m_dataManager->setApiKey(m_apiKeyEdit->text());
    }
    
    // 配置连接参数
    m_dataManager->setNetworkUpdateInterval(m_updateIntervalSpin->value());
    
    // 保存设置
    saveSettings();
    
    QMessageBox::information(this, "设置已保存", "网络配置已成功保存并应用！");
}

void NetworkConfigDialog::onResetToDefaults()
{
    int ret = QMessageBox::question(this, "重置设置", 
                                   "确定要重置所有网络设置为默认值吗？",
                                   QMessageBox::Yes | QMessageBox::No);
    
    if (ret == QMessageBox::Yes) {
        // 恢复默认值
        m_serverUrlEdit->setText("http://localhost:8080");
        m_serverPortSpin->setValue(8080);
        m_apiEndpointEdit->setText("/api/teams");
        m_dataSourceCombo->setCurrentIndex(0);
        
        m_authTypeCombo->setCurrentIndex(0);
        m_usernameEdit->clear();
        m_passwordEdit->clear();
        m_apiKeyEdit->clear();
        
        m_updateIntervalSpin->setValue(5000);
        m_autoReconnectCheck->setChecked(true);
        m_maxRetriesSpin->setValue(3);
        m_retryIntervalSpin->setValue(5);
        m_sslVerifyCheck->setChecked(true);
        
        onDataSourceChanged();
    }
}

void NetworkConfigDialog::onDataSourceChanged()
{
    int authType = m_authTypeCombo->currentIndex();
    
    // 根据认证类型显示/隐藏相应控件
    m_usernameEdit->setEnabled(authType == 1);
    m_passwordEdit->setEnabled(authType == 1);
    m_apiKeyEdit->setEnabled(authType == 2);
}

void NetworkConfigDialog::updateConnectionStatus()
{
    if (m_dataManager->isNetworkConnected()) {
        m_statusLabel->setText("已连接");
        m_statusLabel->setStyleSheet("color: #27ae60;");
        m_latencyLabel->setText(QString("%1 ms").arg(m_dataManager->networkLatency()));
        m_lastUpdateLabel->setText(m_dataManager->lastRefreshTime().toString("yyyy-MM-dd hh:mm:ss"));
    } else {
        m_statusLabel->setText("未连接");
        m_statusLabel->setStyleSheet("color: #e74c3c;");
        m_latencyLabel->setText("未知");
    }
    
    m_progressBar->setVisible(false);
    m_testButton->setEnabled(true);
}

void NetworkConfigDialog::onNetworkError(const QString &error)
{
    m_statusLabel->setText("连接失败");
    m_statusLabel->setStyleSheet("color: #e74c3c;");
    m_progressBar->setVisible(false);
    m_testButton->setEnabled(true);
    
    QMessageBox::warning(this, "连接错误", QString("网络连接失败：\n%1").arg(error));
}

void NetworkConfigDialog::onConnectionStatusChanged(NetworkManager::ConnectionStatus status)
{
    switch (status) {
    case NetworkManager::Connected:
        m_statusLabel->setText("已连接");
        m_statusLabel->setStyleSheet("color: #27ae60;");
        break;
    case NetworkManager::Connecting:
        m_statusLabel->setText("连接中...");
        m_statusLabel->setStyleSheet("color: #f39c12;");
        break;
    case NetworkManager::Disconnected:
        m_statusLabel->setText("未连接");
        m_statusLabel->setStyleSheet("color: #e74c3c;");
        break;
    case NetworkManager::Error:
        m_statusLabel->setText("连接错误");
        m_statusLabel->setStyleSheet("color: #e74c3c;");
        break;
    }
    
    m_progressBar->setVisible(false);
    m_testButton->setEnabled(true);
}
