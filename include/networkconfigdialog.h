#ifndef NETWORKCONFIGDIALOG_H
#define NETWORKCONFIGDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QProgressBar>
#include "datamanager.h"
#include "networkmanager.h"

/**
 * @brief 网络配置对话框
 * 
 * 提供网络连接的各种配置选项：
 * - 服务器地址和端口
 * - API认证配置
 * - 数据源类型选择
 * - 连接测试功能
 */
class NetworkConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NetworkConfigDialog(DataManager *dataManager, QWidget *parent = nullptr);

private slots:
    void onTestConnection();
    void onApplySettings();
    void onResetToDefaults();
    void onConnectionStatusChanged(NetworkManager::ConnectionStatus status);
    void onNetworkError(const QString &error);
    void onDataSourceChanged();

private:
    void setupUI();
    void loadCurrentSettings();
    void saveSettings();
    void updateConnectionStatus();
    
    // UI组件
    DataManager *m_dataManager;
    
    // 服务器配置
    QGroupBox *m_serverGroup;
    QLineEdit *m_serverUrlEdit;
    QSpinBox *m_serverPortSpin;
    QLineEdit *m_apiEndpointEdit;
    QComboBox *m_dataSourceCombo;
    
    // 认证配置
    QGroupBox *m_authGroup;
    QLineEdit *m_usernameEdit;
    QLineEdit *m_passwordEdit;
    QLineEdit *m_apiKeyEdit;
    QComboBox *m_authTypeCombo;
    
    // 连接配置
    QGroupBox *m_connectionGroup;
    QSpinBox *m_updateIntervalSpin;
    QCheckBox *m_autoReconnectCheck;
    QSpinBox *m_maxRetriesSpin;
    QSpinBox *m_retryIntervalSpin;
    QCheckBox *m_sslVerifyCheck;
    
    // 状态和控制
    QLabel *m_statusLabel;
    QProgressBar *m_progressBar;
    QPushButton *m_testButton;
    QPushButton *m_applyButton;
    QPushButton *m_resetButton;
    QPushButton *m_closeButton;
    
    // 状态信息
    QLabel *m_latencyLabel;
    QLabel *m_lastUpdateLabel;
    QLabel *m_bytesLabel;
};

#endif // NETWORKCONFIGDIALOG_H
