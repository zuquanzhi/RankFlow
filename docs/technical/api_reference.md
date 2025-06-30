# RankingSystem API 参考文档

## API 概述

RankingSystem 提供了丰富的 API 接口，支持数据访问、界面控制和功能扩展。本文档详细描述了所有公开的 API 接口。

## 核心数据 API

### TeamData 类

#### 构造函数

```cpp
// 默认构造函数
TeamData();

// 从文件构造
TeamData(const QString& filePath);

// 拷贝构造函数
TeamData(const TeamData& other);
```

#### 数据访问方法

```cpp
// 基本信息
QString teamName() const;
void setTeamName(const QString& name);

int totalScore() const;
void setTotalScore(int score);

int solvedProblems() const;
double accuracy() const;
QDateTime lastSubmissionTime() const;

// 提交记录
QList<Submission> submissions() const;
void addSubmission(const Submission& submission);
void removeSubmission(int index);
Submission submissionAt(int index) const;
int submissionCount() const;
```

#### 文件操作方法

```cpp
// 数据加载和保存
bool loadFromFile(const QString& filePath);
bool saveToFile(const QString& filePath) const;

// 数据格式转换
QJsonObject toJson() const;
bool fromJson(const QJsonObject& json);

QString toCsv() const;
bool fromCsv(const QString& csv);
```

#### 数据验证方法

```cpp
// 数据完整性校验
bool isValid() const;
QString calculateHash() const;
bool verifyHash(const QString& expectedHash) const;

// 数据统计
QMap<QString, int> problemStatistics() const;
QList<int> scoreHistory() const;
double averageSubmissionTime() const;
```

### Submission 结构体

```cpp
struct Submission {
    QString problemId;        // 题目ID
    int score;               // 获得分数
    QDateTime timestamp;     // 提交时间
    QString status;          // 状态 (AC/WA/TLE/CE/RE)
    int penalty;            // 罚时（分钟）
    int attempts;           // 尝试次数
    QString language;       // 编程语言
    int executionTime;      // 执行时间（毫秒）
    int memoryUsage;        // 内存使用（KB）
    
    // 构造函数
    Submission();
    Submission(const QString& problemId, int score, const QDateTime& timestamp);
    
    // 序列化
    QJsonObject toJson() const;
    void fromJson(const QJsonObject& json);
    
    // 比较运算符
    bool operator==(const Submission& other) const;
    bool operator<(const Submission& other) const;
};
```

## 数据管理 API

### DataManager 类

#### 构造和配置

```cpp
// 构造函数
explicit DataManager(QObject* parent = nullptr);

// 配置方法
void setDataDirectory(const QString& directory);
QString dataDirectory() const;

void setAutoRefresh(bool enabled);
bool autoRefreshEnabled() const;

void setRefreshInterval(int seconds);
int refreshInterval() const;
```

#### 数据加载方法

```cpp
// 批量数据加载
bool loadAllData();
bool loadData(const QString& pattern = "*.json");
int loadedTeamCount() const;

// 单个队伍加载
bool loadTeam(const QString& filePath);
bool loadTeam(const QString& teamName, const QString& filePath);

// 网络数据加载
bool loadFromUrl(const QUrl& url);
bool loadFromAPI(const QString& apiEndpoint, const QJsonObject& params = {});
```

#### 数据访问方法

```cpp
// 队伍数据访问
QList<TeamData> allTeams() const;
TeamData team(const QString& teamName) const;
TeamData teamAt(int index) const;
int teamCount() const;

// 队伍查找
QList<TeamData> findTeams(const QString& pattern) const;
QList<TeamData> teamsWithScore(int minScore, int maxScore = -1) const;
QList<TeamData> teamsWithProblems(int minProblems, int maxProblems = -1) const;

// 排序和筛选
QList<TeamData> sortedTeams(SortOrder order) const;
QList<TeamData> topTeams(int count) const;
```

#### 数据修改方法

```cpp
// 队伍管理
bool addTeam(const TeamData& team);
bool updateTeam(const TeamData& team);
bool removeTeam(const QString& teamName);

// 提交管理
bool addSubmission(const QString& teamName, const Submission& submission);
bool updateSubmission(const QString& teamName, int index, const Submission& submission);
bool removeSubmission(const QString& teamName, int index);
```

#### 事件和信号

```cpp
signals:
    // 数据更新信号
    void dataUpdated();
    void teamAdded(const TeamData& team);
    void teamUpdated(const TeamData& team);
    void teamRemoved(const QString& teamName);
    
    // 加载进度信号
    void loadingStarted();
    void loadingProgress(int percent);
    void loadingFinished();
    
    // 错误信号
    void errorOccurred(const QString& error);
    void warningOccurred(const QString& warning);
    
    // 文件监控信号
    void fileChanged(const QString& filePath);
    void directoryChanged(const QString& path);
```

## 界面控制 API

### MainWindow 类

#### 窗口控制

```cpp
// 窗口状态
void showFullScreen();
void showNormal();
void showMaximized();
void showMinimized();

bool isFullScreen() const;
bool isMaximized() const;

// 主题控制
void setTheme(Theme theme);
Theme currentTheme() const;

enum Theme {
    StandardTheme,
    DarkTheme,
    HighContrastTheme
};
```

#### 界面组件访问

```cpp
// 获取子组件
QTableView* rankingTable() const;
ChartWidget* chartWidget() const;
ProblemWidget* problemWidget() const;
DanmakuWidget* danmakuWidget() const;

// 工具栏和菜单
QToolBar* mainToolBar() const;
QMenuBar* mainMenuBar() const;
QStatusBar* mainStatusBar() const;
```

#### 数据显示控制

```cpp
// 排序控制
void setSortOrder(SortOrder order);
SortOrder currentSortOrder() const;

// 刷新控制
void refreshData();
void setAutoRefresh(bool enabled);
void setRefreshInterval(int seconds);

// 显示选项
void setShowCharts(bool show);
void setShowProblems(bool show);
void setShowDanmaku(bool show);
```

### RankingModel 类

#### QAbstractTableModel 接口

```cpp
// 基本模型接口
int rowCount(const QModelIndex& parent = QModelIndex()) const override;
int columnCount(const QModelIndex& parent = QModelIndex()) const override;

QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

bool setData(const QModelIndex& index, const QVariant& value, int role) override;
Qt::ItemFlags flags(const QModelIndex& index) const override;
```

#### 自定义方法

```cpp
// 数据更新
void updateData(const QList<TeamData>& teams);
void addTeam(const TeamData& team);
void removeTeam(int row);
void updateTeam(int row, const TeamData& team);

// 排序功能
void setSortOrder(SortOrder order);
SortOrder sortOrder() const;

enum SortOrder {
    ByRank,           // 按排名
    ByTotalScore,     // 按总分
    BySolvedProblems, // 按解题数
    ByAccuracy,       // 按准确率
    ByLastSubmission  // 按最后提交时间
};

// 过滤功能
void setFilter(const QString& pattern);
QString filter() const;

void setScoreRange(int minScore, int maxScore);
QPair<int, int> scoreRange() const;
```

## 可视化 API

### ChartWidget 类

#### 图表类型控制

```cpp
// 图表类型
enum ChartType {
    BarChart,        // 柱状图
    LineChart,       // 折线图
    PieChart,        // 饼图
    ScatterChart,    // 散点图
    AreaChart,       // 面积图
    SplineChart      // 曲线图
};

void setChartType(ChartType type);
ChartType chartType() const;
```

#### 数据更新方法

```cpp
// 数据设置
void updateData(const QList<TeamData>& teams);
void setDataSeries(QAbstractSeries* series);

// 图表配置
void setTitle(const QString& title);
void setXAxisTitle(const QString& title);
void setYAxisTitle(const QString& title);

void setLegendVisible(bool visible);
void setAnimationEnabled(bool enabled);
```

#### 图表导出

```cpp
// 图片导出
bool exportToPng(const QString& filePath, const QSize& size = QSize()) const;
bool exportToJpg(const QString& filePath, const QSize& size = QSize()) const;
bool exportToSvg(const QString& filePath, const QSize& size = QSize()) const;

// 数据导出
bool exportToCsv(const QString& filePath) const;
bool exportToJson(const QString& filePath) const;
```

### ProblemWidget 类

#### 题目统计显示

```cpp
// 数据更新
void updateProblems(const QList<TeamData>& teams);
void setProblemList(const QStringList& problems);

// 统计信息获取
QMap<QString, int> problemSubmissions() const;
QMap<QString, int> problemAccepted() const;
QMap<QString, double> problemAcceptanceRate() const;

// 显示控制
void setShowSubmissions(bool show);
void setShowAcceptanceRate(bool show);
void setSortByAcceptance(bool sort);
```

### DanmakuWidget 类

#### 弹幕控制

```cpp
// 弹幕添加
void addDanmaku(const QString& text, const QColor& color = Qt::white);
void addDanmaku(const QString& text, const QColor& color, int speed);

// 弹幕配置
void setDanmakuSpeed(int speed);        // 1-10
void setDanmakuOpacity(double opacity); // 0.0-1.0
void setMaxDanmakuCount(int count);
void setFontSize(int size);

// 控制方法
void start();
void stop();
void pause();
void resume();
void clear();

// 状态查询
bool isRunning() const;
bool isPaused() const;
int danmakuCount() const;
```

## 工具 API

### 文件工具

```cpp
namespace FileUtils {
    // 文件操作
    bool copyFile(const QString& source, const QString& destination);
    bool moveFile(const QString& source, const QString& destination);
    bool removeFile(const QString& filePath);
    
    // 目录操作
    bool createDirectory(const QString& path);
    bool removeDirectory(const QString& path);
    QStringList listFiles(const QString& directory, const QStringList& filters);
    
    // 文件信息
    qint64 fileSize(const QString& filePath);
    QDateTime fileModified(const QString& filePath);
    bool fileExists(const QString& filePath);
    
    // 文件内容
    QString readTextFile(const QString& filePath);
    bool writeTextFile(const QString& filePath, const QString& content);
    QByteArray readBinaryFile(const QString& filePath);
    bool writeBinaryFile(const QString& filePath, const QByteArray& data);
}
```

### 数据转换工具

```cpp
namespace DataUtils {
    // JSON 转换
    QJsonObject teamToJson(const TeamData& team);
    TeamData teamFromJson(const QJsonObject& json);
    
    QJsonArray teamsToJsonArray(const QList<TeamData>& teams);
    QList<TeamData> teamsFromJsonArray(const QJsonArray& array);
    
    // CSV 转换
    QString teamsToCsv(const QList<TeamData>& teams);
    QList<TeamData> teamsFromCsv(const QString& csv);
    
    // 数据验证
    bool isValidTeamData(const QJsonObject& json);
    bool isValidSubmission(const QJsonObject& json);
    
    // 数据统计
    QMap<QString, QVariant> calculateStatistics(const QList<TeamData>& teams);
    QMap<QString, int> problemStatistics(const QList<TeamData>& teams);
}
```

### 时间工具

```cpp
namespace TimeUtils {
    // 时间格式化
    QString formatDateTime(const QDateTime& dateTime);
    QString formatDuration(qint64 milliseconds);
    QString formatRelativeTime(const QDateTime& dateTime);
    
    // 时间解析
    QDateTime parseDateTime(const QString& string);
    qint64 parseDuration(const QString& string);
    
    // 时间计算
    qint64 timeDifference(const QDateTime& start, const QDateTime& end);
    QDateTime addDuration(const QDateTime& dateTime, qint64 milliseconds);
}
```

## 配置 API

### Settings 类

```cpp
class Settings : public QObject {
    Q_OBJECT
public:
    static Settings* instance();
    
    // 设置读写
    QVariant value(const QString& key, const QVariant& defaultValue = QVariant()) const;
    void setValue(const QString& key, const QVariant& value);
    
    // 组操作
    void beginGroup(const QString& prefix);
    void endGroup();
    
    // 配置文件
    bool load(const QString& filePath = QString());
    bool save(const QString& filePath = QString());
    
    // 常用设置
    QString dataDirectory() const;
    void setDataDirectory(const QString& directory);
    
    bool autoRefresh() const;
    void setAutoRefresh(bool enabled);
    
    int refreshInterval() const;
    void setRefreshInterval(int seconds);
    
    Theme theme() const;
    void setTheme(Theme theme);
    
signals:
    void settingChanged(const QString& key, const QVariant& value);
};
```

## 错误处理 API

### 错误代码

```cpp
enum ErrorCode {
    NoError = 0,
    
    // 文件错误 (1000-1999)
    FileNotFound = 1001,
    FilePermissionDenied = 1002,
    FileFormatError = 1003,
    FileCorrupted = 1004,
    
    // 数据错误 (2000-2999)
    InvalidDataFormat = 2001,
    DataValidationFailed = 2002,
    DataIncomplete = 2003,
    DataConflict = 2004,
    
    // 网络错误 (3000-3999)
    NetworkTimeout = 3001,
    NetworkConnectionFailed = 3002,
    NetworkPermissionDenied = 3003,
    NetworkInvalidResponse = 3004,
    
    // 系统错误 (4000-4999)
    InsufficientMemory = 4001,
    SystemPermissionDenied = 4002,
    SystemResourceUnavailable = 4003,
    
    // 用户错误 (5000-5999)
    InvalidInput = 5001,
    OperationCancelled = 5002,
    OperationNotAllowed = 5003
};
```

### 错误处理类

```cpp
class Error {
public:
    Error(ErrorCode code = NoError, const QString& message = QString());
    
    ErrorCode code() const;
    QString message() const;
    QString codeString() const;
    
    bool isError() const { return m_code != NoError; }
    operator bool() const { return isError(); }
    
    static QString errorString(ErrorCode code);
    
private:
    ErrorCode m_code;
    QString m_message;
};

// 使用示例
Error loadTeamData(const QString& filePath) {
    if (!QFile::exists(filePath)) {
        return Error(FileNotFound, "Team data file not found: " + filePath);
    }
    
    // 加载数据...
    
    return Error(); // 成功，返回无错误状态
}
```

## 事件系统 API

### 自定义事件

```cpp
// 数据更新事件
class DataUpdateEvent : public QEvent {
public:
    static const QEvent::Type Type;
    
    DataUpdateEvent(const QList<TeamData>& teams);
    QList<TeamData> teams() const;
    
private:
    QList<TeamData> m_teams;
};

// 队伍变化事件
class TeamChangeEvent : public QEvent {
public:
    enum ChangeType { Added, Updated, Removed };
    
    TeamChangeEvent(ChangeType type, const TeamData& team);
    ChangeType changeType() const;
    TeamData team() const;
    
private:
    ChangeType m_type;
    TeamData m_team;
};
```

### 事件处理

```cpp
class EventHandler : public QObject {
    Q_OBJECT
protected:
    bool event(QEvent* event) override;
    
private:
    void handleDataUpdate(DataUpdateEvent* event);
    void handleTeamChange(TeamChangeEvent* event);
};
```

## 扩展插件 API

### 插件接口

```cpp
class IPlugin {
public:
    virtual ~IPlugin() = default;
    
    // 基本信息
    virtual QString name() const = 0;
    virtual QString version() const = 0;
    virtual QString description() const = 0;
    virtual QStringList dependencies() const = 0;
    
    // 生命周期
    virtual bool initialize(QObject* application) = 0;
    virtual void finalize() = 0;
    
    // 功能接口
    virtual QWidget* createWidget(QWidget* parent = nullptr) { return nullptr; }
    virtual QMenu* createMenu(QWidget* parent = nullptr) { return nullptr; }
    virtual QList<QAction*> createActions(QObject* parent = nullptr) { return {}; }
};

// 插件工厂宏
#define RANKING_PLUGIN_INTERFACE_IID "com.rankingsystem.plugin.IPlugin/1.0"
Q_DECLARE_INTERFACE(IPlugin, RANKING_PLUGIN_INTERFACE_IID)
```

### 插件管理器

```cpp
class PluginManager : public QObject {
    Q_OBJECT
public:
    static PluginManager* instance();
    
    // 插件加载
    bool loadPlugin(const QString& filePath);
    bool loadPlugins(const QString& directory);
    void unloadPlugin(const QString& name);
    void unloadAllPlugins();
    
    // 插件查询
    QStringList availablePlugins() const;
    IPlugin* plugin(const QString& name) const;
    bool isPluginLoaded(const QString& name) const;
    
signals:
    void pluginLoaded(IPlugin* plugin);
    void pluginUnloaded(const QString& name);
    void pluginError(const QString& name, const QString& error);
};
```

---

**API 版本：** v1.0  
**适用系统版本：** RankingSystem v1.0+  
**最后更新：** 2025年6月30日
