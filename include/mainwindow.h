#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableView>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QSpinBox>
#include <QSplitter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QProgressBar>
#include <QStatusBar>
#include <QMenuBar>
#include <QToolBar>
#include <QAction>

#include "rankingmodel.h"
#include "datamanager.h"
#include "chartwidget.h"
#include "problemwidget.h"
#include "danmakuwidget.h"
#include "querydialog.h"
#include "networkconfigdialog.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onRefreshButtonClicked();
    void onAutoRefreshToggled(bool enabled);
    void onRefreshIntervalChanged(int seconds);
    void onSortTypeChanged(int index);
    void onDataRefreshed();
    void onRefreshStarted();
    void onRefreshFinished();
    void onErrorOccurred(const QString &error);
    
    // 菜单操作
    void onOpenDataDirectory();
    void onViewAuditLog();
    void onAbout();
    void onFullScreen();
    
    // 新增的查询功能
    void onOpenQueryDialog();
    
    // 网络配置功能
    void onOpenNetworkConfig();
    void onDataSourceChanged(int index);
    void onNetworkConnected();
    void onNetworkDisconnected();
    void onNetworkError(const QString &error);
    
    // 表格操作
    void onTableSelectionChanged();

private:
    void setupUI();
    void setupMenuBar();
    void setupStatusBar();
    void setupControlPanel();
    void setupMainLayout();
    void connectSignals();
    void updateStatusBar();
    void loadSettings();
    void saveSettings();
    
    // UI组件
    QWidget *m_centralWidget;
    QSplitter *m_mainSplitter;
    QSplitter *m_rightSplitter;
    
    // 左侧排行榜
    QGroupBox *m_rankingGroup;
    QTableView *m_rankingTable;
    RankingModel *m_rankingModel;
    
    // 右上角图表
    QGroupBox *m_chartGroup;
    ChartWidget *m_chartWidget;
    
    // 右下角题目状态
    QGroupBox *m_problemGroup;
    ProblemWidget *m_problemWidget;
    
    // 弹幕区域
    QGroupBox *m_danmakuGroup;
    DanmakuWidget *m_danmakuWidget;
    
    // 控制面板
    QGroupBox *m_controlGroup;
    QPushButton *m_refreshButton;
    QLabel *m_refreshStatusLabel;
    QComboBox *m_sortTypeCombo;
    QSpinBox *m_refreshIntervalSpinBox;
    QPushButton *m_autoRefreshButton;
    QProgressBar *m_refreshProgressBar;
    
    // 网络控制组件
    QComboBox *m_dataSourceCombo;
    QLabel *m_networkStatusLabel;
    
    // 状态显示
    QLabel *m_titleLabel;
    QLabel *m_lastRefreshLabel;
    QLabel *m_teamCountLabel;
    QLabel *m_topTeamLabel;
    
    // 数据管理
    DataManager *m_dataManager;
    
    // 菜单
    QAction *m_openDataDirAction;
    QAction *m_viewLogAction;
    QAction *m_fullScreenAction;
    QAction *m_aboutAction;
    QAction *m_exitAction;
    QAction *m_queryAction;  // 新增的查询菜单项

    // 状态
    bool m_isFullScreen;
    bool m_autoRefreshEnabled;
};

#endif // MAINWINDOW_H
