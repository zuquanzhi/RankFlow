# 技术文档

## 竞赛排行榜系统技术架构

### 1. 总体架构

#### 1.1 技术栈
- **UI框架**: Qt 5.15+
- **图表库**: Qt Charts
- **构建系统**: CMake 3.16+
- **编程语言**: C++17
- **数据格式**: JSON

#### 1.2 架构模式
系统采用经典的MVC架构模式：
- **Model**: `TeamData`, `RankingModel` - 数据模型层
- **View**: `MainWindow`, `ChartWidget`, `ProblemWidget`, `DanmakuWidget` - 视图层
- **Controller**: `DataManager` - 控制器层

### 2. 核心模块

#### 2.1 数据模型 (TeamData)

**职责：**
- 封装单个队伍的所有数据
- 提供数据序列化和反序列化
- 计算统计信息（通过题数、准确率等）
- 支持文件完整性校验

**关键方法：**
```cpp
// 数据操作
void addSubmission(const Submission &submission);
bool loadFromFile(const QString &filePath);
bool saveToFile(const QString &filePath) const;

// 统计计算
int solvedProblems() const;
double accuracy() const;
int averageTime() const;

// 校验功能
QString calculateHash() const;
bool verifyHash(const QString &hashFilePath) const;
```

#### 2.2 排行榜模型 (RankingModel)

**职责：**
- 管理所有队伍数据的显示
- 提供多种排序算法
- 实现Qt表格模型接口
- 处理UI数据绑定

**排序算法：**
```cpp
enum SortType {
    SortByScore = 0,    // 按总分排序（默认）
    SortBySolved,       // 按通过题数排序
    SortByTime,         // 按提交时间排序
    SortByAccuracy      // 按准确率排序
};
```

**关键特性：**
- 前三名特殊颜色标识
- 并列排名处理
- 动态数据更新
- 高效排序算法

#### 2.3 数据管理器 (DataManager)

**职责：**
- 监控数据文件变化
- 自动/手动数据刷新
- 文件完整性验证
- 操作审计日志

**核心功能：**
```cpp
// 数据管理
void setDataDirectory(const QString &path);
void refreshData();
bool loadTeamData(const QString &teamId);

// 自动刷新
void setRefreshInterval(int seconds);
void setAutoRefresh(bool enabled);

// 文件监控
QFileSystemWatcher *m_fileWatcher;
void onFileChanged(const QString &path);
```

#### 2.4 图表组件 (ChartWidget)

**职责：**
- 数据可视化展示
- 多种图表类型支持
- 交互式图表操作
- 队伍高亮显示

**图表类型：**
```cpp
enum ChartType {
    ScoreChart = 0,     // 得分柱状图
    AccuracyChart,      // 准确率折线图
    TimeChart,          // 响应时间图
    ProblemChart        // 题目通过率饼图
};
```

### 3. 数据流处理

#### 3.1 数据加载流程

```
数据文件 → DataManager → TeamData → RankingModel → UI显示
    ↓           ↓           ↓           ↓
文件监控 → 完整性校验 → 数据解析 → 排序处理 → 界面更新
```

#### 3.2 自动刷新机制

```cpp
class DataManager {
private:
    QTimer *m_refreshTimer;              // 定时器
    QFileSystemWatcher *m_fileWatcher;   // 文件监控器
    
public slots:
    void onRefreshTimer();               // 定时刷新
    void onFileChanged(const QString &path); // 文件变化刷新
};
```

#### 3.3 数据校验流程

```
JSON文件 → SHA256计算 → 与校验文件比对 → 验证结果
    ↓           ↓            ↓           ↓
加载数据 → 生成哈希 → 读取存储哈希 → 通过/失败
```

### 4. 界面设计

#### 4.1 布局结构

```
MainWindow
├── QSplitter (水平)
│   ├── 排行榜区域 (QGroupBox)
│   │   └── QTableView
│   └── QSplitter (垂直)
│       ├── 图表区域 (QGroupBox)
│       │   └── ChartWidget
│       ├── 题目状态区域 (QGroupBox)
│       │   └── ProblemWidget
│       └── 弹幕区域 (QGroupBox)
│           └── DanmakuWidget
```

#### 4.2 响应式设计

- 使用QSplitter实现可调整布局
- 支持窗口缩放和全屏显示
- 自适应不同屏幕分辨率
- 保存和恢复窗口状态

#### 4.3 主题系统

```cpp
// 深色主题配置
QPalette darkPalette;
darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
darkPalette.setColor(QPalette::WindowText, Qt::white);
// ... 更多颜色配置
app.setPalette(darkPalette);
```

### 5. 性能优化

#### 5.1 数据处理优化

**策略：**
- 增量数据更新，避免全量刷新
- 限制图表显示数量（前10-15名）
- 异步文件操作，避免UI阻塞
- 智能排序，只在数据变化时重排

**实现：**
```cpp
void RankingModel::updateTeam(const TeamData &team) {
    // 只更新变化的队伍数据
    for (int i = 0; i < m_teams.size(); ++i) {
        if (m_teams[i].teamId() == team.teamId()) {
            m_teams[i] = team;
            // 发出最小范围的数据变化信号
            emit dataChanged(createIndex(i, 0), createIndex(i, ColumnCount - 1));
            return;
        }
    }
}
```

#### 5.2 内存管理

**策略：**
- 限制弹幕消息数量（最多1000条）
- 及时释放图表资源
- 使用智能指针管理对象生命周期
- 定期清理临时数据

#### 5.3 文件I/O优化

**策略：**
- 异步文件读取
- 文件变化批量处理（延迟1秒）
- 缓存文件修改时间，避免重复读取
- 使用QFileSystemWatcher提高响应性

### 6. 错误处理

#### 6.1 异常处理策略

```cpp
// 文件读取错误处理
bool TeamData::loadFromFile(const QString &filePath) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "无法打开文件:" << filePath;
        return false;
    }
    
    QByteArray data = file.readAll();
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(data, &error);
    
    if (error.error != QJsonParseError::NoError) {
        qDebug() << "JSON解析错误:" << error.errorString();
        emit errorOccurred(QString("JSON解析错误: %1").arg(error.errorString()));
        return false;
    }
    
    // 继续处理...
}
```

#### 6.2 数据验证

**验证层级：**
1. 文件存在性检查
2. JSON格式验证
3. 数据字段完整性检查
4. SHA256完整性校验
5. 业务逻辑验证

#### 6.3 日志系统

```cpp
// 审计日志记录
void DataManager::addAuditEntry(const QString &entry) {
    QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
    QString logEntry = QString("[%1] %2").arg(timestamp, entry);
    
    m_auditLog.append(logEntry);
    
    // 写入文件
    QString logFilePath = QString("logs/audit_%1.log")
                         .arg(QDate::currentDate().toString("yyyy-MM-dd"));
    QFile logFile(logFilePath);
    if (logFile.open(QIODevice::WriteOnly | QIODevice::Append)) {
        logFile.write((logEntry + "\n").toUtf8());
    }
}
```

### 7. 扩展性设计

#### 7.1 插件架构预留

```cpp
// 为未来插件系统预留接口
class IDataProvider {
public:
    virtual ~IDataProvider() = default;
    virtual QList<TeamData> loadTeams() = 0;
    virtual bool isSupported(const QString &source) = 0;
};

class IChartRenderer {
public:
    virtual ~IChartRenderer() = default;
    virtual QWidget* createChart(const QList<TeamData> &data) = 0;
    virtual QString chartType() const = 0;
};
```

#### 7.2 配置系统

```cpp
// 配置管理
class ConfigManager {
public:
    static ConfigManager& instance();
    
    void setValue(const QString &key, const QVariant &value);
    QVariant value(const QString &key, const QVariant &defaultValue = QVariant()) const;
    
private:
    QSettings *m_settings;
};
```

#### 7.3 国际化支持

项目结构预留国际化支持：
```
translations/
├── ranking_zh_CN.ts
├── ranking_en_US.ts
└── ranking_ja_JP.ts
```

### 8. 部署和分发

#### 8.1 构建系统

**CMake配置特点：**
- 跨平台支持（Windows/macOS/Linux）
- 自动依赖检测
- 多配置支持（Debug/Release）
- 资源文件自动处理

#### 8.2 打包策略

**Windows:**
```bash
# 使用windeployqt自动打包Qt依赖
windeployqt.exe RankingSystem.exe
```

**macOS:**
```bash
# 使用macdeployqt创建应用包
macdeployqt RankingSystem.app
```

**Linux:**
```bash
# 使用AppImage或静态链接
linuxdeployqt RankingSystem -appimage
```

### 9. 测试策略

#### 9.1 单元测试

```cpp
// 示例测试用例
class TestTeamData : public QObject {
    Q_OBJECT
    
private slots:
    void testDataLoading();
    void testScoreCalculation();
    void testSerialization();
};

void TestTeamData::testScoreCalculation() {
    TeamData team("test", "Test Team");
    
    Submission sub1;
    sub1.problemId = "A";
    sub1.isCorrect = true;
    team.addSubmission(sub1);
    
    QCOMPARE(team.totalScore(), 100);
    QCOMPARE(team.solvedProblems(), 1);
}
```

#### 9.2 集成测试

- 数据文件加载测试
- UI组件交互测试
- 自动刷新功能测试
- 图表渲染测试

#### 9.3 性能测试

- 大数据量加载测试（100+队伍）
- 长时间运行稳定性测试
- 内存泄漏检测
- UI响应时间测试

### 10. 维护指南

#### 10.1 代码规范

- 遵循Qt编码规范
- 使用现代C++特性（C++17）
- 注重异常安全和资源管理
- 保持良好的文档和注释

#### 10.2 版本管理

- 语义化版本号（Semantic Versioning）
- 详细的变更日志
- 向后兼容性考虑
- 数据格式版本控制

#### 10.3 监控和诊断

- 详细的日志记录
- 性能指标收集
- 错误报告机制
- 用户行为分析

---

本技术文档将随项目发展持续更新，确保开发和维护团队能够有效协作。
