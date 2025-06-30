#include "querydialog.h"
#include <QMessageBox>
#include <QFileDialog>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QApplication>
#include <QHeaderView>

QueryDialog::QueryDialog(DataManager *dataManager, QWidget *parent)
    : QDialog(parent)
    , m_dataManager(dataManager)
    , m_resultsModel(nullptr)
{
    setWindowTitle("高级查询 - 基于二叉树的快速查询");
    setMinimumSize(800, 600);
    resize(1000, 700);
    
    setupUI();
    updateQueryOptions();
    
    // 创建结果模型
    m_resultsModel = new RankingModel(this);
    m_resultsTable->setModel(m_resultsModel);
    
    // 设置表格样式
    m_resultsTable->setAlternatingRowColors(true);
    m_resultsTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_resultsTable->horizontalHeader()->setStretchLastSection(true);
    m_resultsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
}

QueryDialog::~QueryDialog()
{
}

void QueryDialog::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);
    
    // 查询选项组
    m_queryOptionsGroup = new QGroupBox("查询选项", this);
    m_optionsLayout = new QGridLayout(m_queryOptionsGroup);
    
    // 查询类型选择
    m_optionsLayout->addWidget(new QLabel("查询类型:"), 0, 0);
    m_queryTypeCombo = new QComboBox();
    m_queryTypeCombo->addItems({
        "按标准排序",
        "获取前N名队伍",
        "获取后N名队伍", 
        "分数范围查询",
        "按队伍名称搜索",
        "按解题数搜索",
        "按准确率搜索",
        "查询队伍排名",
        "统计信息"
    });
    m_optionsLayout->addWidget(m_queryTypeCombo, 0, 1);
    
    // 排序标准
    m_optionsLayout->addWidget(new QLabel("排序标准:"), 1, 0);
    m_sortCriteriaCombo = new QComboBox();
    m_sortCriteriaCombo->addItems({
        "队伍ID",
        "队伍名称",
        "总分",
        "最后提交时间",
        "解题数量",
        "准确率"
    });
    m_sortCriteriaCombo->setCurrentIndex(2); // 默认按总分排序
    m_optionsLayout->addWidget(m_sortCriteriaCombo, 1, 1);
    
    // 队伍名称搜索
    m_optionsLayout->addWidget(new QLabel("队伍名称:"), 2, 0);
    m_nameSearchEdit = new QLineEdit();
    m_nameSearchEdit->setPlaceholderText("支持通配符，如: team* 或 *test*");
    m_optionsLayout->addWidget(m_nameSearchEdit, 2, 1);
    
    // 数量限制
    m_optionsLayout->addWidget(new QLabel("数量:"), 3, 0);
    m_countSpinBox = new QSpinBox();
    m_countSpinBox->setRange(1, 1000);
    m_countSpinBox->setValue(10);
    m_optionsLayout->addWidget(m_countSpinBox, 3, 1);
    
    // 分数范围
    m_optionsLayout->addWidget(new QLabel("最低分数:"), 4, 0);
    m_minScoreSpinBox = new QSpinBox();
    m_minScoreSpinBox->setRange(0, 10000);
    m_minScoreSpinBox->setValue(0);
    m_optionsLayout->addWidget(m_minScoreSpinBox, 4, 1);
    
    m_optionsLayout->addWidget(new QLabel("最高分数:"), 5, 0);
    m_maxScoreSpinBox = new QSpinBox();
    m_maxScoreSpinBox->setRange(0, 10000);
    m_maxScoreSpinBox->setValue(1000);
    m_optionsLayout->addWidget(m_maxScoreSpinBox, 5, 1);
    
    // 最少解题数
    m_optionsLayout->addWidget(new QLabel("最少解题数:"), 6, 0);
    m_minSolvedSpinBox = new QSpinBox();
    m_minSolvedSpinBox->setRange(0, 100);
    m_minSolvedSpinBox->setValue(1);
    m_optionsLayout->addWidget(m_minSolvedSpinBox, 6, 1);
    
    // 最低准确率
    m_optionsLayout->addWidget(new QLabel("最低准确率:"), 7, 0);
    m_minAccuracySpinBox = new QDoubleSpinBox();
    m_minAccuracySpinBox->setRange(0.0, 1.0);
    m_minAccuracySpinBox->setDecimals(2);
    m_minAccuracySpinBox->setSingleStep(0.01);
    m_minAccuracySpinBox->setValue(0.0);
    m_minAccuracySpinBox->setSuffix(" (0.0-1.0)");
    m_optionsLayout->addWidget(m_minAccuracySpinBox, 7, 1);
    
    // 队伍ID查询
    m_optionsLayout->addWidget(new QLabel("队伍ID:"), 8, 0);
    m_teamIdEdit = new QLineEdit();
    m_teamIdEdit->setPlaceholderText("输入要查询排名的队伍ID");
    m_optionsLayout->addWidget(m_teamIdEdit, 8, 1);
    
    m_mainLayout->addWidget(m_queryOptionsGroup);
    
    // 控制按钮
    m_controlLayout = new QHBoxLayout();
    m_executeButton = new QPushButton("执行查询");
    m_clearButton = new QPushButton("清空结果");
    m_exportButton = new QPushButton("导出结果");
    
    m_controlLayout->addWidget(m_executeButton);
    m_controlLayout->addWidget(m_clearButton);
    m_controlLayout->addWidget(m_exportButton);
    m_controlLayout->addStretch();
    
    m_mainLayout->addLayout(m_controlLayout);
    
    // 结果显示组
    m_resultsGroup = new QGroupBox("查询结果", this);
    QVBoxLayout *resultsLayout = new QVBoxLayout(m_resultsGroup);
    
    // 状态标签
    m_statusLabel = new QLabel("就绪");
    m_statusLabel->setStyleSheet("QLabel { color: blue; font-weight: bold; }");
    resultsLayout->addWidget(m_statusLabel);
    
    // 统计信息标签
    m_statisticsLabel = new QLabel();
    m_statisticsLabel->setWordWrap(true);
    m_statisticsLabel->setStyleSheet("QLabel { background-color: #f0f0f0; padding: 10px; border: 1px solid #ccc; border-radius: 5px; }");
    resultsLayout->addWidget(m_statisticsLabel);
    
    // 结果表格
    m_resultsTable = new QTableView();
    resultsLayout->addWidget(m_resultsTable);
    
    m_mainLayout->addWidget(m_resultsGroup);
    
    // 连接信号
    connect(m_queryTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &QueryDialog::onQueryTypeChanged);
    connect(m_executeButton, &QPushButton::clicked, this, &QueryDialog::onExecuteQuery);
    connect(m_clearButton, &QPushButton::clicked, this, &QueryDialog::onClearResults);
    connect(m_exportButton, &QPushButton::clicked, this, &QueryDialog::onExportResults);
}

void QueryDialog::updateQueryOptions()
{
    QueryType type = static_cast<QueryType>(m_queryTypeCombo->currentIndex());
    
    // 隐藏所有选项
    for (int i = 1; i < m_optionsLayout->rowCount(); ++i) {
        for (int j = 0; j < m_optionsLayout->columnCount(); ++j) {
            QLayoutItem *item = m_optionsLayout->itemAtPosition(i, j);
            if (item && item->widget()) {
                item->widget()->hide();
            }
        }
    }
    
    // 根据查询类型显示相关选项
    switch (type) {
        case SortBy:
            m_optionsLayout->itemAtPosition(1, 0)->widget()->show(); // 排序标准标签
            m_optionsLayout->itemAtPosition(1, 1)->widget()->show(); // 排序标准组合框
            break;
            
        case TopTeams:
        case BottomTeams:
            m_optionsLayout->itemAtPosition(3, 0)->widget()->show(); // 数量标签
            m_optionsLayout->itemAtPosition(3, 1)->widget()->show(); // 数量输入框
            break;
            
        case ScoreRange:
            m_optionsLayout->itemAtPosition(4, 0)->widget()->show(); // 最低分数标签
            m_optionsLayout->itemAtPosition(4, 1)->widget()->show(); // 最低分数输入框
            m_optionsLayout->itemAtPosition(5, 0)->widget()->show(); // 最高分数标签
            m_optionsLayout->itemAtPosition(5, 1)->widget()->show(); // 最高分数输入框
            break;
            
        case SearchByName:
            m_optionsLayout->itemAtPosition(2, 0)->widget()->show(); // 队伍名称标签
            m_optionsLayout->itemAtPosition(2, 1)->widget()->show(); // 队伍名称输入框
            break;
            
        case SearchBySolvedProblems:
            m_optionsLayout->itemAtPosition(6, 0)->widget()->show(); // 最少解题数标签
            m_optionsLayout->itemAtPosition(6, 1)->widget()->show(); // 最少解题数输入框
            break;
            
        case SearchByAccuracy:
            m_optionsLayout->itemAtPosition(7, 0)->widget()->show(); // 最低准确率标签
            m_optionsLayout->itemAtPosition(7, 1)->widget()->show(); // 最低准确率输入框
            break;
            
        case TeamRank:
            m_optionsLayout->itemAtPosition(8, 0)->widget()->show(); // 队伍ID标签
            m_optionsLayout->itemAtPosition(8, 1)->widget()->show(); // 队伍ID输入框
            break;
            
        case Statistics:
            // 统计信息不需要额外参数
            break;
    }
}

void QueryDialog::onQueryTypeChanged(int index)
{
    Q_UNUSED(index)
    updateQueryOptions();
}

void QueryDialog::onExecuteQuery()
{
    if (!m_dataManager) {
        QMessageBox::warning(this, "错误", "数据管理器未初始化");
        return;
    }
    
    m_statusLabel->setText("正在执行查询...");
    QApplication::processEvents();
    
    executeQuery();
    
    m_statusLabel->setText("查询完成");
}

void QueryDialog::executeQuery()
{
    QueryType type = static_cast<QueryType>(m_queryTypeCombo->currentIndex());
    QList<TeamData> results;
    
    switch (type) {
        case SortBy: {
            TeamQueryTree::SortCriteria criteria = 
                static_cast<TeamQueryTree::SortCriteria>(m_sortCriteriaCombo->currentIndex());
            results = m_dataManager->getTeamsSortedBy(criteria);
            break;
        }
        
        case TopTeams:
            results = m_dataManager->getTopTeamsByScore(m_countSpinBox->value());
            break;
            
        case BottomTeams:
            results = m_dataManager->getBottomTeamsByScore(m_countSpinBox->value());
            break;
            
        case ScoreRange:
            results = m_dataManager->getTeamsInScoreRange(
                m_minScoreSpinBox->value(), m_maxScoreSpinBox->value());
            break;
            
        case SearchByName:
            if (!m_nameSearchEdit->text().isEmpty()) {
                results = m_dataManager->searchTeamsByName(m_nameSearchEdit->text());
            }
            break;
            
        case SearchBySolvedProblems:
            results = m_dataManager->searchTeamsBySolvedProblems(m_minSolvedSpinBox->value());
            break;
            
        case SearchByAccuracy:
            results = m_dataManager->searchTeamsByAccuracy(m_minAccuracySpinBox->value());
            break;
            
        case TeamRank:
            if (!m_teamIdEdit->text().isEmpty()) {
                int rank = m_dataManager->getTeamRank(m_teamIdEdit->text());
                if (rank > 0) {
                    TeamData team = m_dataManager->getTeam(m_teamIdEdit->text());
                    if (!team.teamId().isEmpty()) {
                        results.append(team);
                        m_statusLabel->setText(QString("队伍 %1 当前排名: 第 %2 名")
                                             .arg(m_teamIdEdit->text()).arg(rank));
                    }
                } else {
                    m_statusLabel->setText("未找到指定队伍或队伍未参与排名");
                }
            }
            break;
            
        case Statistics:
            displayStatistics();
            return;
    }
    
    displayResults(results);
}

void QueryDialog::displayResults(const QList<TeamData>& teams)
{
    if (m_resultsModel) {
        m_resultsModel->setTeamData(teams);
        
        QString info = QString("查询完成，共找到 %1 支队伍").arg(teams.size());
        if (!teams.isEmpty()) {
            int totalScore = 0;
            for (const TeamData& team : teams) {
                totalScore += team.totalScore();
            }
            double avgScore = static_cast<double>(totalScore) / teams.size();
            info += QString("，平均分: %1").arg(avgScore, 0, 'f', 2);
        }
        m_statusLabel->setText(info);
    }
}

void QueryDialog::displayStatistics()
{
    if (!m_dataManager) return;
    
    double avgScore = m_dataManager->getAverageScore();
    int medianScore = m_dataManager->getMedianScore();
    int totalTeams = m_dataManager->totalTeams();
    
    QStringList problems = m_dataManager->availableProblems();
    
    QString stats = QString(
        "数据库统计信息:\n"
        "• 总队伍数: %1\n"
        "• 平均分数: %2\n"
        "• 中位数分数: %3\n"
        "• 可用题目数: %4\n"
        "• 题目列表: %5"
    ).arg(totalTeams)
     .arg(avgScore, 0, 'f', 2)
     .arg(medianScore)
     .arg(problems.size())
     .arg(problems.join(", "));
    
    m_statisticsLabel->setText(stats);
    m_statisticsLabel->show();
    
    // 清空表格
    if (m_resultsModel) {
        m_resultsModel->setTeamData(QList<TeamData>());
    }
}

void QueryDialog::onClearResults()
{
    if (m_resultsModel) {
        m_resultsModel->setTeamData(QList<TeamData>());
    }
    m_statusLabel->setText("就绪");
    m_statisticsLabel->clear();
    m_statisticsLabel->hide();
}

void QueryDialog::onExportResults()
{
    if (!m_resultsModel || m_resultsModel->rowCount() == 0) {
        QMessageBox::information(this, "提示", "没有查询结果可以导出");
        return;
    }
    
    QString fileName = QFileDialog::getSaveFileName(
        this, "导出查询结果", 
        QString("query_results_%1.json").arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss")),
        "JSON文件 (*.json)");
    
    if (fileName.isEmpty()) {
        return;
    }
    
    // 构建JSON数据
    QJsonArray resultsArray;
    for (int i = 0; i < m_resultsModel->rowCount(); ++i) {
        // 这里需要根据RankingModel的实际实现来获取数据
        // 简化版本，实际使用时需要根据模型调整
        QJsonObject teamObj;
        teamObj["rank"] = i + 1;
        // 添加其他字段...
        resultsArray.append(teamObj);
    }
    
    QJsonObject exportData;
    exportData["query_type"] = m_queryTypeCombo->currentText();
    exportData["export_time"] = QDateTime::currentDateTime().toString(Qt::ISODate);
    exportData["total_results"] = resultsArray.size();
    exportData["results"] = resultsArray;
    
    QJsonDocument doc(exportData);
    
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
        QMessageBox::information(this, "成功", QString("查询结果已导出到: %1").arg(fileName));
    } else {
        QMessageBox::warning(this, "错误", "无法写入文件");
    }
}
