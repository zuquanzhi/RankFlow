#ifndef CHARTWIDGET_H
#define CHARTWIDGET_H

#include <QWidget>
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QLineSeries>
#include <QtCharts/QPieSeries>
#include <QtCharts/QValueAxis>
#include <QtCharts/QBarCategoryAxis>
#include <QComboBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include "teamdata.h"

QT_CHARTS_USE_NAMESPACE

class ChartWidget : public QWidget
{
    Q_OBJECT

public:
    enum ChartType {
        ScoreChart = 0,
        AccuracyChart,
        TimeChart,
        ProblemChart
    };

    explicit ChartWidget(QWidget *parent = nullptr);
    
    void updateData(const QList<TeamData> &teams);
    void highlightTeam(const QString &teamId);

public slots:
    void onChartTypeChanged(int type);

private:
    void setupUI();
    void createScoreChart(const QList<TeamData> &teams);
    void createAccuracyChart(const QList<TeamData> &teams);
    void createTimeChart(const QList<TeamData> &teams);
    void createProblemChart(const QList<TeamData> &teams);
    
    QChartView *m_chartView;
    QChart *m_chart;
    QComboBox *m_chartTypeCombo;
    QLabel *m_chartTitleLabel;
    
    QList<TeamData> m_teams;
    QString m_highlightedTeam;
    ChartType m_currentType;
};

#endif // CHARTWIDGET_H
