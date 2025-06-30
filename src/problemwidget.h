#ifndef PROBLEMWIDGET_H
#define PROBLEMWIDGET_H

#include <QWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>
#include "teamdata.h"

class ProblemWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProblemWidget(QWidget *parent = nullptr);
    
    void updateProblems(const QStringList &problems, const QList<TeamData> &teams);

private slots:
    void onExportClicked();
    void onRefreshClicked();

private:
    void setupUI();
    void updateStatistics();
    
    QTableWidget *m_problemTable;
    QLabel *m_totalProblemsLabel;
    QLabel *m_avgSolveRateLabel;
    QLabel *m_hardestProblemLabel;
    QPushButton *m_exportButton;
    QPushButton *m_refreshButton;
    
    QStringList m_problems;
    QList<TeamData> m_teams;
};

#endif // PROBLEMWIDGET_H
