#include "chartwidget.h"
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QRandomGenerator>

ChartWidget::ChartWidget(QWidget *parent)
    : QWidget(parent)
    , m_chartView(nullptr)
    , m_chart(nullptr)
    , m_currentType(ScoreChart)
{
    setupUI();
}

void ChartWidget::setupUI()
{
    QVBoxLayout *layout = new QVBoxLayout;
    
    // 图表类型选择
    QHBoxLayout *controlLayout = new QHBoxLayout;
    m_chartTitleLabel = new QLabel("数据可视化");
    m_chartTitleLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    
    m_chartTypeCombo = new QComboBox;
    m_chartTypeCombo->addItem("得分分布", ScoreChart);
    m_chartTypeCombo->addItem("准确率对比", AccuracyChart);
    m_chartTypeCombo->addItem("响应时间", TimeChart);
    m_chartTypeCombo->addItem("题目通过率", ProblemChart);
    
    controlLayout->addWidget(m_chartTitleLabel);
    controlLayout->addStretch();
    controlLayout->addWidget(new QLabel("图表类型:"));
    controlLayout->addWidget(m_chartTypeCombo);
    
    // 创建图表视图
    m_chart = new QChart;
    m_chart->setTheme(QChart::ChartThemeDark);
    m_chart->setAnimationOptions(QChart::SeriesAnimations);
    
    m_chartView = new QChartView(m_chart);
    m_chartView->setRenderHint(QPainter::Antialiasing);
    
    layout->addLayout(controlLayout);
    layout->addWidget(m_chartView);
    setLayout(layout);
    
    // 连接信号
    connect(m_chartTypeCombo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &ChartWidget::onChartTypeChanged);
}

void ChartWidget::updateData(const QList<TeamData> &teams)
{
    m_teams = teams;
    
    switch (m_currentType) {
    case ScoreChart:
        createScoreChart(teams);
        break;
    case AccuracyChart:
        createAccuracyChart(teams);
        break;
    case TimeChart:
        createTimeChart(teams);
        break;
    case ProblemChart:
        createProblemChart(teams);
        break;
    }
}

void ChartWidget::highlightTeam(const QString &teamId)
{
    m_highlightedTeam = teamId;
    updateData(m_teams); // 重新绘制以高亮显示
}

void ChartWidget::onChartTypeChanged(int type)
{
    m_currentType = static_cast<ChartType>(type);
    updateData(m_teams);
}

void ChartWidget::createScoreChart(const QList<TeamData> &teams)
{
    m_chart->removeAllSeries();
    
    // 清除现有坐标轴
    foreach(QAbstractAxis *axis, m_chart->axes()) {
        m_chart->removeAxis(axis);
    }
    
    if (teams.isEmpty()) {
        m_chart->setTitle("暂无数据");
        return;
    }
    
    // 创建柱状图
    QBarSeries *series = new QBarSeries;
    QBarSet *scoreSet = new QBarSet("总分");
    QBarSet *solvedSet = new QBarSet("通过题数");
    
    QStringList categories;
    
    // 只显示前10名，避免图表过于拥挤
    int maxTeams = qMin(10, teams.size());
    for (int i = 0; i < maxTeams; ++i) {
        const TeamData &team = teams[i];
        
        *scoreSet << team.totalScore();
        *solvedSet << team.solvedProblems() * 20; // 乘以20使其在图表上可见
        
        categories << team.teamName();
        
        // 高亮显示选中的队伍
        if (team.teamId() == m_highlightedTeam) {
            scoreSet->setBrush(QColor(255, 215, 0)); // 金色高亮
        }
    }
    
    series->append(scoreSet);
    series->append(solvedSet);
    
    m_chart->addSeries(series);
    m_chart->setTitle("队伍得分对比 (前10名)");
    
    // 设置坐标轴
    QBarCategoryAxis *axisX = new QBarCategoryAxis;
    axisX->append(categories);
    m_chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    
    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(0, 1000);
    axisY->setTitleText("分数");
    m_chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
}

void ChartWidget::createAccuracyChart(const QList<TeamData> &teams)
{
    m_chart->removeAllSeries();
    
    // 清除现有坐标轴
    foreach(QAbstractAxis *axis, m_chart->axes()) {
        m_chart->removeAxis(axis);
    }
    
    if (teams.isEmpty()) {
        m_chart->setTitle("暂无数据");
        return;
    }
    
    // 创建折线图显示准确率趋势
    QLineSeries *series = new QLineSeries;
    series->setName("准确率 (%)");
    
    int maxTeams = qMin(15, teams.size());
    for (int i = 0; i < maxTeams; ++i) {
        const TeamData &team = teams[i];
        series->append(i + 1, team.accuracy());
    }
    
    m_chart->addSeries(series);
    m_chart->setTitle("队伍准确率趋势");
    
    // 设置坐标轴
    QValueAxis *axisX = new QValueAxis;
    axisX->setRange(1, maxTeams);
    axisX->setTitleText("排名");
    axisX->setLabelFormat("%d");
    m_chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    
    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(0, 100);
    axisY->setTitleText("准确率 (%)");
    m_chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
}

void ChartWidget::createTimeChart(const QList<TeamData> &teams)
{
    m_chart->removeAllSeries();
    
    // 清除现有坐标轴
    foreach(QAbstractAxis *axis, m_chart->axes()) {
        m_chart->removeAxis(axis);
    }
    
    if (teams.isEmpty()) {
        m_chart->setTitle("暂无数据");
        return;
    }
    
    // 创建柱状图显示平均响应时间
    QBarSeries *series = new QBarSeries;
    QBarSet *timeSet = new QBarSet("平均响应时间 (ms)");
    
    QStringList categories;
    
    int maxTeams = qMin(10, teams.size());
    for (int i = 0; i < maxTeams; ++i) {
        const TeamData &team = teams[i];
        
        *timeSet << team.averageTime();
        categories << team.teamName();
        
        // 高亮显示选中的队伍
        if (team.teamId() == m_highlightedTeam) {
            timeSet->setBrush(QColor(255, 215, 0));
        }
    }
    
    series->append(timeSet);
    
    m_chart->addSeries(series);
    m_chart->setTitle("平均响应时间对比");
    
    // 设置坐标轴
    QBarCategoryAxis *axisX = new QBarCategoryAxis;
    axisX->append(categories);
    m_chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);
    
    QValueAxis *axisY = new QValueAxis;
    axisY->setRange(0, 5000); // 假设最大5秒响应时间
    axisY->setTitleText("时间 (ms)");
    m_chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);
}

void ChartWidget::createProblemChart(const QList<TeamData> &teams)
{
    m_chart->removeAllSeries();
    
    // 清除现有坐标轴
    foreach(QAbstractAxis *axis, m_chart->axes()) {
        m_chart->removeAxis(axis);
    }
    
    if (teams.isEmpty()) {
        m_chart->setTitle("暂无数据");
        return;
    }
    
    // 统计各题目的通过情况
    QMap<QString, int> problemStats;
    QStringList allProblems;
    
    for (const TeamData &team : teams) {
        for (const Submission &submission : team.submissions()) {
            if (!allProblems.contains(submission.problemId)) {
                allProblems.append(submission.problemId);
                problemStats[submission.problemId] = 0;
            }
            
            if (submission.isCorrect) {
                problemStats[submission.problemId]++;
            }
        }
    }
    
    if (allProblems.isEmpty()) {
        m_chart->setTitle("暂无题目数据");
        return;
    }
    
    // 创建饼图显示题目通过率
    QPieSeries *series = new QPieSeries;
    
    allProblems.sort();
    for (const QString &problem : allProblems) {
        int solvedCount = problemStats[problem];
        double percentage = teams.isEmpty() ? 0.0 : 
                           (static_cast<double>(solvedCount) / teams.size() * 100.0);
        
        QPieSlice *slice = series->append(QString("题目%1 (%2%)")
                                         .arg(problem)
                                         .arg(QString::number(percentage, 'f', 1)), 
                                         solvedCount);
        
        // 设置颜色
        QColor color = QColor::fromHsv((allProblems.indexOf(problem) * 360 / allProblems.size()) % 360, 
                                      200, 200);
        slice->setBrush(color);
        
        // 如果通过率很低，高亮显示
        if (percentage < 30) {
            slice->setExploded(true);
        }
    }
    
    m_chart->addSeries(series);
    m_chart->setTitle("题目通过率分布");
    
    // 设置标签可见
    for (QPieSlice *slice : series->slices()) {
        slice->setLabelVisible(true);
    }
}
