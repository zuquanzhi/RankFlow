#include "mainwindow.h"
#include "querydialog.h"
#include "networkconfigdialog.h"
#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QFileDialog>
#include <QDesktopServices>
#include <QUrl>
#include <QSettings>
#include <QHeaderView>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_centralWidget(nullptr)
    , m_rankingModel(new RankingModel(this))
    , m_dataManager(new DataManager(this))
    , m_isFullScreen(false)
    , m_autoRefreshEnabled(false)
{
    setupUI();
    setupMenuBar();
    setupStatusBar();
    connectSignals();
    loadSettings();
    
    // 设置窗口标题和图标
    setWindowTitle("竞赛排行榜系统 v1.0");
    setWindowIcon(QIcon(":/icons/app_icon_64x64.png"));
    setMinimumSize(1200, 800);
    resize(1600, 1000);
    
    // 延迟初始化 - 让UI先加载完成
    QTimer::singleShot(500, this, [this]() {
        // 只有在本地文件模式下才自动刷新
        if (m_dataManager->dataSource() == DataManager::LocalFile) {
            m_dataManager->refreshData();
        }
    });
}

MainWindow::~MainWindow()
{
    saveSettings();
}

void MainWindow::setupUI()
{
    setupMainLayout();
    setupControlPanel();
}

void MainWindow::setupMainLayout()
{
    m_centralWidget = new QWidget;
    setCentralWidget(m_centralWidget);
    
    // 创建主分割器
    m_mainSplitter = new QSplitter(Qt::Horizontal);
    
    // 左侧排行榜区域
    m_rankingGroup = new QGroupBox("实时排行榜");
    m_rankingTable = new QTableView;
    m_rankingTable->setModel(m_rankingModel);
    m_rankingTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_rankingTable->setAlternatingRowColors(true);
    m_rankingTable->setSortingEnabled(true);
    m_rankingTable->horizontalHeader()->setStretchLastSection(true);
    m_rankingTable->verticalHeader()->setVisible(false);
    
    // 设置列宽
    m_rankingTable->setColumnWidth(0, 80);  // 排名
    m_rankingTable->setColumnWidth(1, 200); // 队伍名称
    m_rankingTable->setColumnWidth(2, 100); // 总分
    m_rankingTable->setColumnWidth(3, 100); // 通过题数
    m_rankingTable->setColumnWidth(4, 100); // 准确率
    m_rankingTable->setColumnWidth(5, 120); // 最后提交
    
    QVBoxLayout *rankingLayout = new QVBoxLayout;
    rankingLayout->addWidget(m_rankingTable);
    m_rankingGroup->setLayout(rankingLayout);
    
    // 右侧分割器
    m_rightSplitter = new QSplitter(Qt::Vertical);
    
    // 图表区域
    m_chartGroup = new QGroupBox("数据可视化");
    m_chartWidget = new ChartWidget;
    QVBoxLayout *chartLayout = new QVBoxLayout;
    chartLayout->addWidget(m_chartWidget);
    m_chartGroup->setLayout(chartLayout);
    
    // 题目状态区域
    m_problemGroup = new QGroupBox("题目通过情况");
    m_problemWidget = new ProblemWidget;
    QVBoxLayout *problemLayout = new QVBoxLayout;
    problemLayout->addWidget(m_problemWidget);
    m_problemGroup->setLayout(problemLayout);
    
    // 弹幕区域
    m_danmakuGroup = new QGroupBox("观众互动");
    m_danmakuWidget = new DanmakuWidget;
    QVBoxLayout *danmakuLayout = new QVBoxLayout;
    danmakuLayout->addWidget(m_danmakuWidget);
    m_danmakuGroup->setLayout(danmakuLayout);
    
    // 添加到右侧分割器
    m_rightSplitter->addWidget(m_chartGroup);
    m_rightSplitter->addWidget(m_problemGroup);
    m_rightSplitter->addWidget(m_danmakuGroup);
    m_rightSplitter->setStretchFactor(0, 2); // 图表占更多空间
    m_rightSplitter->setStretchFactor(1, 1);
    m_rightSplitter->setStretchFactor(2, 1);
    
    // 添加到主分割器
    m_mainSplitter->addWidget(m_rankingGroup);
    m_mainSplitter->addWidget(m_rightSplitter);
    m_mainSplitter->setStretchFactor(0, 2); // 排行榜占更多空间
    m_mainSplitter->setStretchFactor(1, 1);
    
    // 创建顶部标题和控制区域
    QVBoxLayout *mainLayout = new QVBoxLayout;
    
    // 标题区域
    QHBoxLayout *titleLayout = new QHBoxLayout;
    m_titleLabel = new QLabel("竞赛实时排行榜");
    m_titleLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #3498db;");
    m_titleLabel->setAlignment(Qt::AlignCenter);
    
    m_lastRefreshLabel = new QLabel("最后刷新: 未知");
    m_lastRefreshLabel->setStyleSheet("color: #7f8c8d;");
    
    titleLayout->addWidget(m_titleLabel);
    titleLayout->addStretch();
    titleLayout->addWidget(m_lastRefreshLabel);
    
    mainLayout->addLayout(titleLayout);
    mainLayout->addWidget(m_mainSplitter);
    
    m_centralWidget->setLayout(mainLayout);
}

void MainWindow::setupControlPanel()
{
    // 控制面板 (作为工具栏)
    QWidget *controlWidget = new QWidget;
    QHBoxLayout *controlLayout = new QHBoxLayout;
    
    // 刷新按钮
    m_refreshButton = new QPushButton("手动刷新");
    m_refreshButton->setIcon(style()->standardIcon(QStyle::SP_BrowserReload));
    
    // 自动刷新开关
    m_autoRefreshButton = new QPushButton("启用自动刷新");
    m_autoRefreshButton->setCheckable(true);
    
    // 刷新间隔设置
    QLabel *intervalLabel = new QLabel("刷新间隔(秒):");
    m_refreshIntervalSpinBox = new QSpinBox;
    m_refreshIntervalSpinBox->setRange(30, 3600);
    m_refreshIntervalSpinBox->setValue(600);
    m_refreshIntervalSpinBox->setSuffix(" 秒");
    
    // 排序方式
    QLabel *sortLabel = new QLabel("排序方式:");
    m_sortTypeCombo = new QComboBox;
    m_sortTypeCombo->addItem("按总分排序", RankingModel::SortByScore);
    m_sortTypeCombo->addItem("按通过题数排序", RankingModel::SortBySolved);
    m_sortTypeCombo->addItem("按提交时间排序", RankingModel::SortByTime);
    m_sortTypeCombo->addItem("按准确率排序", RankingModel::SortByAccuracy);
    
    // 状态标签
    m_refreshStatusLabel = new QLabel("就绪");
    m_teamCountLabel = new QLabel("队伍数: 0");
    m_topTeamLabel = new QLabel("领先队伍: 无");
    
    // 进度条
    m_refreshProgressBar = new QProgressBar;
    m_refreshProgressBar->setVisible(false);
    
    // 高级查询按钮
    QPushButton *queryButton = new QPushButton("高级查询");
    queryButton->setToolTip("打开基于二叉树的高级查询对话框 (Ctrl+F)");
    queryButton->setIcon(QIcon(":/icons/app_icon_32x32.png"));
    connect(queryButton, &QPushButton::clicked, this, &MainWindow::onOpenQueryDialog);
    
    // 网络配置按钮
    QPushButton *networkButton = new QPushButton("网络配置");
    networkButton->setToolTip("配置网络数据源和服务器连接");
    networkButton->setIcon(style()->standardIcon(QStyle::SP_ComputerIcon));
    connect(networkButton, &QPushButton::clicked, this, &MainWindow::onOpenNetworkConfig);
    
    // 数据源切换
    QLabel *sourceLabel = new QLabel("数据源:");
    m_dataSourceCombo = new QComboBox;
    m_dataSourceCombo->addItem("本地文件", static_cast<int>(DataManager::LocalFile));
    m_dataSourceCombo->addItem("网络实时", static_cast<int>(DataManager::Network));
    m_dataSourceCombo->addItem("混合模式", static_cast<int>(DataManager::Hybrid));
    connect(m_dataSourceCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onDataSourceChanged);
    
    // 网络状态指示器
    m_networkStatusLabel = new QLabel("网络: 未连接");
    m_networkStatusLabel->setStyleSheet("color: #e74c3c;");
    
    controlLayout->addWidget(m_refreshButton);
    controlLayout->addWidget(m_autoRefreshButton);
    controlLayout->addWidget(intervalLabel);
    controlLayout->addWidget(m_refreshIntervalSpinBox);
    controlLayout->addWidget(new QLabel("|")); // 分隔符
    controlLayout->addWidget(queryButton);
    controlLayout->addWidget(networkButton);    // 网络配置按钮
    controlLayout->addWidget(sourceLabel);      // 数据源标签
    controlLayout->addWidget(m_dataSourceCombo); // 数据源选择
    controlLayout->addWidget(sortLabel);
    controlLayout->addWidget(m_sortTypeCombo);
    controlLayout->addStretch();
    controlLayout->addWidget(m_networkStatusLabel); // 网络状态
    controlLayout->addWidget(m_teamCountLabel);
    controlLayout->addWidget(m_topTeamLabel);
    controlLayout->addWidget(m_refreshStatusLabel);
    controlLayout->addWidget(m_refreshProgressBar);
    
    controlWidget->setLayout(controlLayout);
    
    // 添加为工具栏
    QToolBar *toolBar = addToolBar("控制面板");
    toolBar->addWidget(controlWidget);
    toolBar->setMovable(false);
}

void MainWindow::setupMenuBar()
{
    // 文件菜单
    QMenu *fileMenu = menuBar()->addMenu("文件(&F)");
    
    m_openDataDirAction = new QAction("打开数据目录(&O)", this);
    m_openDataDirAction->setShortcut(QKeySequence("Ctrl+O"));
    fileMenu->addAction(m_openDataDirAction);
    
    fileMenu->addSeparator();
    
    m_exitAction = new QAction("退出(&X)", this);
    m_exitAction->setShortcut(QKeySequence("Ctrl+Q"));
    fileMenu->addAction(m_exitAction);
    
    // 查看菜单
    QMenu *viewMenu = menuBar()->addMenu("查看(&V)");
    
    m_fullScreenAction = new QAction("全屏显示(&F)", this);
    m_fullScreenAction->setShortcut(QKeySequence("F11"));
    m_fullScreenAction->setCheckable(true);
    viewMenu->addAction(m_fullScreenAction);
    
    viewMenu->addSeparator();
    
    m_viewLogAction = new QAction("查看审计日志(&L)", this);
    viewMenu->addAction(m_viewLogAction);
    
    // 查询菜单
    QMenu *queryMenu = menuBar()->addMenu("查询(&Q)");
    
    m_queryAction = new QAction("高级查询(&A)", this);
    m_queryAction->setShortcut(QKeySequence("Ctrl+F"));
    m_queryAction->setToolTip("打开基于二叉树的高级查询对话框");
    queryMenu->addAction(m_queryAction);
    
    // 帮助菜单
    QMenu *helpMenu = menuBar()->addMenu("帮助(&H)");
    
    m_aboutAction = new QAction("关于(&A)", this);
    helpMenu->addAction(m_aboutAction);
}

void MainWindow::setupStatusBar()
{
    statusBar()->showMessage("系统已启动");
}

void MainWindow::connectSignals()
{
    // 数据管理器信号
    connect(m_dataManager, &DataManager::dataRefreshed, this, &MainWindow::onDataRefreshed);
    connect(m_dataManager, &DataManager::refreshStarted, this, &MainWindow::onRefreshStarted);
    connect(m_dataManager, &DataManager::refreshFinished, this, &MainWindow::onRefreshFinished);
    connect(m_dataManager, &DataManager::errorOccurred, this, &MainWindow::onErrorOccurred);
    
    // 控制面板信号
    connect(m_refreshButton, &QPushButton::clicked, this, &MainWindow::onRefreshButtonClicked);
    connect(m_autoRefreshButton, &QPushButton::toggled, this, &MainWindow::onAutoRefreshToggled);
    connect(m_refreshIntervalSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::onRefreshIntervalChanged);
    connect(m_sortTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onSortTypeChanged);
    
    // 网络相关信号
    connect(m_dataSourceCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onDataSourceChanged);
    connect(m_dataManager, &DataManager::networkConnected, 
            this, &MainWindow::onNetworkConnected);
    connect(m_dataManager, &DataManager::networkDisconnected,
            this, &MainWindow::onNetworkDisconnected);
    connect(m_dataManager, &DataManager::networkErrorOccurred,
            this, &MainWindow::onNetworkError);
    
    // 表格信号
    connect(m_rankingTable->selectionModel(), &QItemSelectionModel::selectionChanged,
            this, &MainWindow::onTableSelectionChanged);
    
    // 菜单信号
    connect(m_openDataDirAction, &QAction::triggered, this, &MainWindow::onOpenDataDirectory);
    connect(m_viewLogAction, &QAction::triggered, this, &MainWindow::onViewAuditLog);
    connect(m_fullScreenAction, &QAction::triggered, this, &MainWindow::onFullScreen);
    connect(m_aboutAction, &QAction::triggered, this, &MainWindow::onAbout);
    connect(m_exitAction, &QAction::triggered, this, &QWidget::close);
    connect(m_queryAction, &QAction::triggered, this, &MainWindow::onOpenQueryDialog);
    
    // 排行榜模型信号
    connect(m_rankingModel, &RankingModel::dataUpdated, this, &MainWindow::updateStatusBar);
}

void MainWindow::onRefreshButtonClicked()
{
    m_dataManager->refreshData();
}

void MainWindow::onAutoRefreshToggled(bool enabled)
{
    m_autoRefreshEnabled = enabled;
    m_dataManager->setAutoRefresh(enabled);
    
    if (enabled) {
        m_autoRefreshButton->setText("停止自动刷新");
        m_autoRefreshButton->setStyleSheet("background-color: #e74c3c; color: white;");
    } else {
        m_autoRefreshButton->setText("启用自动刷新");
        m_autoRefreshButton->setStyleSheet("");
    }
}

void MainWindow::onRefreshIntervalChanged(int seconds)
{
    m_dataManager->setRefreshInterval(seconds);
}

void MainWindow::onSortTypeChanged(int index)
{
    RankingModel::SortType sortType = static_cast<RankingModel::SortType>(
        m_sortTypeCombo->itemData(index).toInt());
    m_rankingModel->setSortType(sortType);
}

void MainWindow::onDataRefreshed()
{
    // 更新排行榜数据
    m_rankingModel->setTeamData(m_dataManager->allTeams());
    
    // 更新图表
    m_chartWidget->updateData(m_dataManager->allTeams());
    
    // 更新题目状态
    m_problemWidget->updateProblems(m_dataManager->availableProblems(), 
                                   m_dataManager->allTeams());
    
    // 更新时间显示
    m_lastRefreshLabel->setText(QString("最后刷新: %1")
                               .arg(m_dataManager->lastRefreshTime()
                                   .toString("hh:mm:ss")));
    
    updateStatusBar();
}

void MainWindow::onRefreshStarted()
{
    m_refreshStatusLabel->setText("正在刷新...");
    m_refreshProgressBar->setVisible(true);
    m_refreshProgressBar->setRange(0, 0); // 无限进度条
    m_refreshButton->setEnabled(false);
}

void MainWindow::onRefreshFinished()
{
    m_refreshStatusLabel->setText("刷新完成");
    m_refreshProgressBar->setVisible(false);
    m_refreshButton->setEnabled(true);
    
    // 3秒后恢复为"就绪"状态
    QTimer::singleShot(3000, [this]() {
        m_refreshStatusLabel->setText("就绪");
    });
}

void MainWindow::onErrorOccurred(const QString &error)
{
    m_refreshStatusLabel->setText("错误: " + error);
    statusBar()->showMessage("错误: " + error, 5000);
    
    QMessageBox::warning(this, "错误", error);
}

void MainWindow::onOpenDataDirectory()
{
    QString dir = QFileDialog::getExistingDirectory(this, "选择数据目录", "data");
    if (!dir.isEmpty()) {
        m_dataManager->setDataDirectory(dir);
        statusBar()->showMessage("数据目录已更改为: " + dir, 3000);
    }
}

void MainWindow::onViewAuditLog()
{
    QStringList logs = m_dataManager->getAuditLog();
    
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("审计日志");
    dialog->resize(800, 600);
    
    QTextEdit *textEdit = new QTextEdit;
    textEdit->setPlainText(logs.join("\n"));
    textEdit->setReadOnly(true);
    
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(textEdit);
    dialog->setLayout(layout);
    
    dialog->show();
}

void MainWindow::onAbout()
{
    QMessageBox::about(this, "关于",
        "<h3>竞赛排行榜系统 v1.0</h3>"
        "<p>一个专为编程竞赛设计的实时排行榜展示系统</p>"
        "<p><b>功能特点:</b></p>"
        "<ul>"
        "<li>实时数据刷新</li>"
        "<li>多维度排序</li>"
        "<li>数据可视化</li>"
        "<li>文件完整性校验</li>"
        "<li>操作审计日志</li>"
        "</ul>"
        "<p>基于 Qt5 开发，支持跨平台运行</p>");
}

void MainWindow::onFullScreen()
{
    if (m_isFullScreen) {
        showNormal();
        m_fullScreenAction->setChecked(false);
    } else {
        showFullScreen();
        m_fullScreenAction->setChecked(true);
    }
    m_isFullScreen = !m_isFullScreen;
}

void MainWindow::onTableSelectionChanged()
{
    QModelIndexList selection = m_rankingTable->selectionModel()->selectedRows();
    if (!selection.isEmpty()) {
        int row = selection.first().row();
        TeamData team = m_rankingModel->teamAt(row);
        
        // 更新图表显示选中队伍的详细信息
        m_chartWidget->highlightTeam(team.teamId());
        
        statusBar()->showMessage(QString("选中队伍: %1").arg(team.teamName()), 2000);
    }
}

void MainWindow::updateStatusBar()
{
    int teamCount = m_rankingModel->totalTeams();
    QString topTeam = m_rankingModel->topTeamName();
    
    m_teamCountLabel->setText(QString("队伍数: %1").arg(teamCount));
    m_topTeamLabel->setText(QString("领先队伍: %1").arg(topTeam.isEmpty() ? "无" : topTeam));
    
    statusBar()->showMessage(QString("已载入 %1 支队伍的数据").arg(teamCount));
}

void MainWindow::loadSettings()
{
    QSettings settings;
    
    // 窗口几何
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
    
    // 分割器状态
    m_mainSplitter->restoreState(settings.value("mainSplitter").toByteArray());
    m_rightSplitter->restoreState(settings.value("rightSplitter").toByteArray());
    
    // 数据目录
    QString dataDir = settings.value("dataDirectory", "data").toString();
    m_dataManager->setDataDirectory(dataDir);
    
    // 刷新间隔
    int interval = settings.value("refreshInterval", 600).toInt();
    m_refreshIntervalSpinBox->setValue(interval);
    m_dataManager->setRefreshInterval(interval);
    
    // 自动刷新
    bool autoRefresh = settings.value("autoRefresh", false).toBool();
    m_autoRefreshButton->setChecked(autoRefresh);
    onAutoRefreshToggled(autoRefresh);
    
    // 排序方式
    int sortType = settings.value("sortType", 0).toInt();
    m_sortTypeCombo->setCurrentIndex(sortType);
    
    // 数据源设置
    int dataSource = settings.value("dataSource", static_cast<int>(DataManager::LocalFile)).toInt();
    m_dataSourceCombo->setCurrentIndex(dataSource);
    m_dataManager->setDataSource(static_cast<DataManager::DataSource>(dataSource));
}

void MainWindow::saveSettings()
{
    QSettings settings;
    
    // 窗口几何
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    
    // 分割器状态
    settings.setValue("mainSplitter", m_mainSplitter->saveState());
    settings.setValue("rightSplitter", m_rightSplitter->saveState());
    
    // 数据目录
    settings.setValue("dataDirectory", m_dataManager->allTeams().isEmpty() ? 
                     "data" : "data"); // 这里可以保存实际的数据目录
    
    // 刷新间隔
    settings.setValue("refreshInterval", m_refreshIntervalSpinBox->value());
    
    // 自动刷新
    settings.setValue("autoRefresh", m_autoRefreshEnabled);
    
    // 排序方式
    settings.setValue("sortType", m_sortTypeCombo->currentIndex());
    
    // 数据源
    settings.setValue("dataSource", m_dataSourceCombo->currentIndex());
}

void MainWindow::onOpenQueryDialog()
{
    QueryDialog *queryDialog = new QueryDialog(m_dataManager, this);
    queryDialog->setModal(true);
    queryDialog->exec();
    queryDialog->deleteLater();
}

void MainWindow::onOpenNetworkConfig()
{
    NetworkConfigDialog *networkDialog = new NetworkConfigDialog(m_dataManager, this);
    networkDialog->setModal(true);
    networkDialog->exec();
    networkDialog->deleteLater();
}

void MainWindow::onDataSourceChanged(int index)
{
    DataManager::DataSource source = static_cast<DataManager::DataSource>(
        m_dataSourceCombo->itemData(index).toInt());
    
    m_dataManager->setDataSource(source);
    
    // 更新UI状态
    switch (source) {
    case DataManager::LocalFile:
        m_networkStatusLabel->setText("网络: 本地模式");
        m_networkStatusLabel->setStyleSheet("color: #7f8c8d;");
        break;
    case DataManager::Network:
        m_networkStatusLabel->setText("网络: 网络模式");
        m_networkStatusLabel->setStyleSheet("color: #3498db;");
        break;
    case DataManager::Hybrid:
        m_networkStatusLabel->setText("网络: 混合模式");
        m_networkStatusLabel->setStyleSheet("color: #9b59b6;");
        break;
    }
    
    statusBar()->showMessage(QString("数据源已切换为: %1")
                            .arg(m_dataSourceCombo->currentText()), 3000);
}

void MainWindow::onNetworkConnected()
{
    m_networkStatusLabel->setText(QString("网络: 已连接 (%1ms)")
                                 .arg(m_dataManager->networkLatency()));
    m_networkStatusLabel->setStyleSheet("color: #27ae60;");
    statusBar()->showMessage("网络连接成功", 3000);
}

void MainWindow::onNetworkDisconnected()
{
    m_networkStatusLabel->setText("网络: 连接断开");
    m_networkStatusLabel->setStyleSheet("color: #e74c3c;");
    statusBar()->showMessage("网络连接已断开", 3000);
}

void MainWindow::onNetworkError(const QString &error)
{
    m_networkStatusLabel->setText("网络: 连接错误");
    m_networkStatusLabel->setStyleSheet("color: #e74c3c;");
    statusBar()->showMessage(QString("网络错误: %1").arg(error), 5000);
}
