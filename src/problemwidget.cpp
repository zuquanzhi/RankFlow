#include "problemwidget.h"
#include <QApplication>
#include <QClipboard>
#include <QMessageBox>
#include <QHeaderView>

ProblemWidget::ProblemWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUI();
}

void ProblemWidget::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout;
    
    // 控制按钮
    QHBoxLayout *controlLayout = new QHBoxLayout;
    QLabel *titleLabel = new QLabel("题目通过情况");
    titleLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    
    m_exportButton = new QPushButton("导出");
    m_exportButton->setMaximumWidth(80);
    m_refreshButton = new QPushButton("刷新");
    m_refreshButton->setMaximumWidth(80);
    
    controlLayout->addWidget(titleLabel);
    controlLayout->addStretch();
    controlLayout->addWidget(m_exportButton);
    controlLayout->addWidget(m_refreshButton);
    
    // 统计信息
    QHBoxLayout *statsLayout = new QHBoxLayout;
    m_totalProblemsLabel = new QLabel("总题数: 0");
    m_avgSolveRateLabel = new QLabel("平均通过率: 0%");
    m_hardestProblemLabel = new QLabel("最难题目: 无");
    
    statsLayout->addWidget(m_totalProblemsLabel);
    statsLayout->addWidget(m_avgSolveRateLabel);
    statsLayout->addWidget(m_hardestProblemLabel);
    statsLayout->addStretch();
    
    // 题目表格
    m_problemTable = new QTableWidget;
    m_problemTable->setColumnCount(4);
    
    QStringList headers;
    headers << "题目ID" << "通过人数" << "总提交数" << "通过率";
    m_problemTable->setHorizontalHeaderLabels(headers);
    
    // 设置表格属性
    m_problemTable->horizontalHeader()->setStretchLastSection(true);
    m_problemTable->verticalHeader()->setVisible(false);
    m_problemTable->setAlternatingRowColors(true);
    m_problemTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_problemTable->setSortingEnabled(true);
    
    // 设置列宽
    m_problemTable->setColumnWidth(0, 80);  // 题目ID
    m_problemTable->setColumnWidth(1, 80);  // 通过人数
    m_problemTable->setColumnWidth(2, 80);  // 总提交数
    
    layout->addLayout(controlLayout);
    layout->addLayout(statsLayout);
    layout->addWidget(m_problemTable);
    setLayout(layout);
    
    // 连接信号
    connect(m_exportButton, &QPushButton::clicked, this, &ProblemWidget::onExportClicked);
    connect(m_refreshButton, &QPushButton::clicked, this, &ProblemWidget::onRefreshClicked);
}

void ProblemWidget::updateProblems(const QStringList &problems, const QList<TeamData> &teams)
{
    m_problems = problems;
    m_teams = teams;
    
    // 清空表格
    m_problemTable->setRowCount(0);
    
    if (problems.isEmpty() || teams.isEmpty()) {
        updateStatistics();
        return;
    }
    
    m_problemTable->setRowCount(problems.size());
    
    for (int i = 0; i < problems.size(); ++i) {
        const QString &problemId = problems[i];
        
        // 统计该题目的提交情况
        int solvedCount = 0;
        int totalSubmissions = 0;
        
        for (const TeamData &team : teams) {
            for (const Submission &submission : team.submissions()) {
                if (submission.problemId == problemId) {
                    totalSubmissions++;
                    if (submission.isCorrect) {
                        solvedCount++;
                    }
                }
            }
        }
        
        double solveRate = totalSubmissions > 0 ? 
                          (static_cast<double>(solvedCount) / totalSubmissions * 100.0) : 0.0;
        
        // 设置表格项
        m_problemTable->setItem(i, 0, new QTableWidgetItem(problemId));
        m_problemTable->setItem(i, 1, new QTableWidgetItem(QString::number(solvedCount)));
        m_problemTable->setItem(i, 2, new QTableWidgetItem(QString::number(totalSubmissions)));
        
        QTableWidgetItem *rateItem = new QTableWidgetItem(QString::number(solveRate, 'f', 1) + "%");
        
        // 根据通过率设置颜色
        if (solveRate >= 80) {
            rateItem->setBackground(QColor(46, 204, 113, 100));  // 绿色
        } else if (solveRate >= 50) {
            rateItem->setBackground(QColor(241, 196, 15, 100));  // 黄色
        } else if (solveRate > 0) {
            rateItem->setBackground(QColor(231, 76, 60, 100));   // 红色
        } else {
            rateItem->setBackground(QColor(149, 165, 166, 100)); // 灰色
        }
        
        m_problemTable->setItem(i, 3, rateItem);
        
        // 设置文本居中
        for (int col = 0; col < 4; ++col) {
            QTableWidgetItem *item = m_problemTable->item(i, col);
            if (item) {
                item->setTextAlignment(Qt::AlignCenter);
            }
        }
    }
    
    updateStatistics();
}

void ProblemWidget::updateStatistics()
{
    m_totalProblemsLabel->setText(QString("总题数: %1").arg(m_problems.size()));
    
    if (m_problems.isEmpty() || m_teams.isEmpty()) {
        m_avgSolveRateLabel->setText("平均通过率: 0%");
        m_hardestProblemLabel->setText("最难题目: 无");
        return;
    }
    
    double totalSolveRate = 0.0;
    double minSolveRate = 100.0;
    QString hardestProblem;
    
    for (const QString &problemId : m_problems) {
        int solvedCount = 0;
        int totalSubmissions = 0;
        
        for (const TeamData &team : m_teams) {
            for (const Submission &submission : team.submissions()) {
                if (submission.problemId == problemId) {
                    totalSubmissions++;
                    if (submission.isCorrect) {
                        solvedCount++;
                    }
                }
            }
        }
        
        double solveRate = totalSubmissions > 0 ? 
                          (static_cast<double>(solvedCount) / totalSubmissions * 100.0) : 0.0;
        
        totalSolveRate += solveRate;
        
        if (solveRate < minSolveRate) {
            minSolveRate = solveRate;
            hardestProblem = problemId;
        }
    }
    
    double avgSolveRate = m_problems.size() > 0 ? totalSolveRate / m_problems.size() : 0.0;
    
    m_avgSolveRateLabel->setText(QString("平均通过率: %1%")
                                .arg(QString::number(avgSolveRate, 'f', 1)));
    
    m_hardestProblemLabel->setText(QString("最难题目: %1 (%2%)")
                                  .arg(hardestProblem.isEmpty() ? "无" : hardestProblem)
                                  .arg(QString::number(minSolveRate, 'f', 1)));
}

void ProblemWidget::onExportClicked()
{
    if (m_problemTable->rowCount() == 0) {
        QMessageBox::information(this, "提示", "暂无数据可导出");
        return;
    }
    
    QString exportText = "题目ID\t通过人数\t总提交数\t通过率\n";
    
    for (int row = 0; row < m_problemTable->rowCount(); ++row) {
        QStringList rowData;
        for (int col = 0; col < m_problemTable->columnCount(); ++col) {
            QTableWidgetItem *item = m_problemTable->item(row, col);
            rowData << (item ? item->text() : "");
        }
        exportText += rowData.join("\t") + "\n";
    }
    
    QApplication::clipboard()->setText(exportText);
    QMessageBox::information(this, "导出成功", "题目统计数据已复制到剪贴板");
}

void ProblemWidget::onRefreshClicked()
{
    updateProblems(m_problems, m_teams);
}
