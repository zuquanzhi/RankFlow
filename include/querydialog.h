#ifndef QUERYDIALOG_H
#define QUERYDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QTableView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QSplitter>
#include "datamanager.h"
#include "rankingmodel.h"

class QueryDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QueryDialog(DataManager *dataManager, QWidget *parent = nullptr);
    ~QueryDialog();

private slots:
    void onQueryTypeChanged(int index);
    void onExecuteQuery();
    void onClearResults();
    void onExportResults();

private:
    enum QueryType {
        SortBy = 0,
        TopTeams,
        BottomTeams,
        ScoreRange,
        SearchByName,
        SearchBySolvedProblems,
        SearchByAccuracy,
        TeamRank,
        Statistics
    };

    void setupUI();
    void updateQueryOptions();
    void executeQuery();
    void displayResults(const QList<TeamData>& teams);
    void displayStatistics();

    // UI组件
    QComboBox *m_queryTypeCombo;
    QComboBox *m_sortCriteriaCombo;
    QLineEdit *m_nameSearchEdit;
    QSpinBox *m_countSpinBox;
    QSpinBox *m_minScoreSpinBox;
    QSpinBox *m_maxScoreSpinBox;
    QSpinBox *m_minSolvedSpinBox;
    QDoubleSpinBox *m_minAccuracySpinBox;
    QLineEdit *m_teamIdEdit;
    
    QPushButton *m_executeButton;
    QPushButton *m_clearButton;
    QPushButton *m_exportButton;
    
    QTableView *m_resultsTable;
    QLabel *m_statusLabel;
    QLabel *m_statisticsLabel;
    
    // 数据和模型
    DataManager *m_dataManager;
    RankingModel *m_resultsModel;
    
    // 布局
    QVBoxLayout *m_mainLayout;
    QHBoxLayout *m_controlLayout;
    QGridLayout *m_optionsLayout;
    QGroupBox *m_queryOptionsGroup;
    QGroupBox *m_resultsGroup;
};

#endif // QUERYDIALOG_H
