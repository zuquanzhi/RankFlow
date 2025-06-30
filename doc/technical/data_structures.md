# Qt5/C++17 编程竞赛排行榜系统 - 数据结构分析

本文档详细分析了编程竞赛排行榜系统中使用的各种数据结构。

## 📊 数据结构概览

### 1. 主要数据结构分类

| 类别 | 数据结构 | 用途 | 文件位置 |
|-----|---------|------|---------|
| **线性结构** | `QList<T>` | 存储队伍列表、提交记录等 | 多个文件 |
| **关联结构** | `QMap<QString, int>` | 题目统计映射 | chartwidget.cpp |
| **字符串结构** | `QStringList` | 存储字符串列表 | 多个文件 |
| **自定义结构** | `struct Submission` | 提交记录结构体 | teamdata.h |
| **时间结构** | `QDateTime` | 时间戳存储 | 多个文件 |

## 🏗️ 核心数据结构详解

### 1. Submission 结构体

```cpp
struct Submission {
    QString problemId;      // 题目ID
    QDateTime timestamp;    // 提交时间戳
    bool isCorrect;        // 是否正确
    int runTime;           // 运行时间(ms)
    int memoryUsage;       // 内存使用(bytes)
    
    Submission() : isCorrect(false), runTime(0), memoryUsage(0) {}
    
    QJsonObject toJson() const;
    void fromJson(const QJsonObject &json);
};
```

**设计特点：**
- ✅ **轻量级结构**：使用简单的POD类型
- ✅ **序列化支持**：提供JSON转换方法
- ✅ **默认构造**：安全的默认值初始化

**用途：**
- 存储单次提交的完整信息
- JSON格式数据交换
- 统计分析的基础单元

### 2. TeamData 类中的数据结构

#### 2.1 核心成员变量
```cpp
class TeamData {
private:
    QString m_teamId;                    // 队伍ID (字符串)
    QString m_teamName;                  // 队伍名称 (字符串)
    QList<Submission> m_submissions;     // 提交记录列表 (动态数组)
    int m_totalScore;                    // 总分 (整数)
    QDateTime m_lastSubmitTime;          // 最后提交时间 (时间对象)
};
```

**QList<Submission> 分析：**
- **容器类型**：Qt的动态数组容器
- **元素类型**：自定义Submission结构体
- **操作特性**：
  - 支持随机访问 O(1)
  - 尾部插入 O(1) 均摊
  - 中间插入/删除 O(n)
  - 内存连续存储

#### 2.2 数据操作方法
```cpp
// 添加提交 - O(1) 均摊复杂度
void addSubmission(const Submission &submission);

// 获取提交列表 - O(1) 复杂度
QList<Submission> submissions() const;

// 统计已解决题目 - O(n) 复杂度
int solvedProblems() const;

// 检查题目状态 - O(n) 复杂度
bool isProblemSolved(const QString &problemId) const;
```

### 3. DataManager 类中的数据结构

#### 3.1 主要容器
```cpp
class DataManager {
private:
    QList<TeamData> m_teams;         // 所有队伍数据
    QStringList m_auditLog;          // 审计日志列表
    QString m_dataDirectory;         // 数据目录路径
    QDateTime m_lastRefreshTime;     // 最后刷新时间
};
```

**QList<TeamData> 特性：**
- **存储对象**：完整的TeamData对象
- **内存管理**：Qt自动管理内存
- **性能特点**：
  - 查找特定队伍：O(n)
  - 添加新队伍：O(1) 均摊
  - 批量更新：O(n)

#### 3.2 查找和统计方法
```cpp
// 获取所有队伍 - O(1) 返回引用
QList<TeamData> allTeams() const;

// 查找特定队伍 - O(n) 线性查找
TeamData getTeam(const QString &teamId) const;

// 获取可用题目列表 - O(n*m) 复杂度
QStringList availableProblems() const;
```

### 4. RankingModel 类中的数据结构

#### 4.1 排行榜数据存储
```cpp
class RankingModel : public QAbstractTableModel {
private:
    QList<TeamData> m_teams;         // 队伍数据副本
    SortType m_sortType;            // 排序类型枚举
};
```

#### 4.2 排序算法实现
```cpp
// 使用STL算法进行排序 - O(n log n) 复杂度
void RankingModel::sortData() {
    std::sort(m_teams.begin(), m_teams.end(), 
              [this](const TeamData &a, const TeamData &b) {
        switch (m_sortType) {
            case SortByScore:
                return a.totalScore() > b.totalScore();
            case SortBySolved:
                return a.solvedProblems() > b.solvedProblems();
            // ... 其他排序条件
        }
    });
}
```

### 5. ChartWidget 类中的数据结构

#### 5.1 图表数据处理
```cpp
class ChartWidget {
private:
    QList<TeamData> m_teams;         // 本地队伍数据缓存
    QString m_highlightedTeam;       // 高亮队伍ID
    ChartType m_currentType;         // 当前图表类型
};
```

#### 5.2 统计数据结构
```cpp
// 在createProblemChart方法中使用的映射结构
void ChartWidget::createProblemChart(const QList<TeamData> &teams) {
    QMap<QString, int> problemStats;  // 题目统计映射
    
    // 统计每个题目的解决次数 - O(n*m) 复杂度
    for (const auto &team : teams) {
        for (const auto &submission : team.submissions()) {
            if (submission.isCorrect) {
                problemStats[submission.problemId]++;
            }
        }
    }
}
```

**QMap<QString, int> 特性：**
- **容器类型**：Qt的关联容器（基于红黑树）
- **键类型**：QString（题目ID）
- **值类型**：int（解决次数）
- **操作复杂度**：
  - 插入/查找：O(log n)
  - 遍历：O(n)
  - 自动排序：按键值字典序

### 6. DanmakuWidget 类中的数据结构

#### 6.1 弹幕系统数据
```cpp
class DanmakuWidget {
private:
    QStringList m_encourageMessages; // 鼓励消息模板列表
    QStringList m_userNames;        // 用户名列表
    int m_messageCount;             // 消息计数器
    bool m_autoScroll;              // 自动滚动标志
    bool m_filterEnabled;           // 过滤器启用标志
};
```

**QStringList 特点：**
- 继承自 `QList<QString>`
- 提供字符串特有的便利方法
- 支持连接、分割、过滤等操作

## 🚀 数据结构性能分析

### 1. 时间复杂度总结

| 操作 | 数据结构 | 复杂度 | 说明 |
|------|---------|--------|------|
| 添加队伍 | `QList<TeamData>` | O(1) 均摊 | 尾部追加 |
| 查找队伍 | `QList<TeamData>` | O(n) | 线性查找 |
| 排序队伍 | `QList<TeamData>` | O(n log n) | STL sort算法 |
| 添加提交 | `QList<Submission>` | O(1) 均摊 | 尾部追加 |
| 统计解题 | `QList<Submission>` | O(n) | 遍历所有提交 |
| 题目统计 | `QMap<QString, int>` | O(log n) | 红黑树插入/查找 |

### 2. 空间复杂度分析

| 组件 | 主要数据 | 空间复杂度 | 估算大小 |
|------|---------|-----------|---------|
| TeamData | 提交记录列表 | O(s) | s = 提交数量 |
| DataManager | 所有队伍数据 | O(t × s) | t = 队伍数，s = 平均提交数 |
| RankingModel | 队伍数据副本 | O(t × s) | 模型层缓存 |
| ChartWidget | 图表数据缓存 | O(t) | 队伍基本信息 |

## 🔧 数据结构选择原则

### 1. 设计决策分析

#### 选择 QList 而非其他容器的原因：
✅ **优点：**
- Qt原生容器，与框架集成良好
- 支持隐式共享，内存效率高
- 提供丰富的STL兼容接口
- 随机访问性能好

❌ **缺点：**
- 查找特定元素需要O(n)时间
- 中间插入/删除性能较差

#### 为什么使用 QMap 而非 QHash：
✅ **QMap 优势：**
- 保持键的有序性（对题目统计有意义）
- 内存使用更可预测
- 迭代器稳定性好

❌ **QHash 劣势：**
- 无序存储不利于统计展示
- 哈希冲突可能影响性能

### 2. 性能优化建议

#### 当前可优化点：
1. **队伍查找优化**：
   ```cpp
   // 当前实现 - O(n)
   TeamData getTeam(const QString &teamId) const;
   
   // 建议优化 - O(log n) 或 O(1)
   QMap<QString, TeamData> m_teamMap;  // 或 QHash
   ```

2. **提交记录查询优化**：
   ```cpp
   // 当前实现 - O(n) 每次查询
   bool isProblemSolved(const QString &problemId) const;
   
   // 建议优化 - 缓存已解决题目
   QSet<QString> m_solvedProblems;  // 缓存解决的题目
   ```

3. **批量更新优化**：
   ```cpp
   // 建议使用预留空间
   m_teams.reserve(expectedTeamCount);
   m_submissions.reserve(expectedSubmissionCount);
   ```

## 📊 内存布局分析

### 1. 对象大小估算

```cpp
// Submission 结构体大小估算
sizeof(Submission) ≈ 
    sizeof(QString) +      // ~24 bytes (指针+长度+容量)
    sizeof(QDateTime) +    // ~8 bytes 
    sizeof(bool) +         // ~1 byte
    sizeof(int) * 2 +      // ~8 bytes
    padding               // ~7 bytes 对齐
    ≈ 48 bytes
```

### 2. 容器开销

```cpp
// QList<TeamData> 内存开销
QList overhead ≈ 24 bytes (Qt 5.x)
+ (capacity × sizeof(TeamData*))  // 指针数组
+ actual TeamData objects         // 实际对象数据
```

## 🔍 数据一致性保证

### 1. 数据同步机制

```cpp
// DataManager 中的数据一致性
void DataManager::refreshData() {
    QList<TeamData> newTeams;  // 临时容器
    
    // 加载新数据到临时容器
    if (loadAllTeams(newTeams)) {
        m_teams = std::move(newTeams);  // 原子替换
        emit dataRefreshed();
    }
}
```

### 2. 线程安全考虑

当前系统主要在主线程运行，但考虑以下安全措施：
- 数据更新时使用信号槽机制
- 避免直接暴露内部容器
- 提供const引用访问方法

## 📈 扩展性设计

### 1. 未来可能的优化方向

1. **引入缓存层**：
   ```cpp
   class DataCache {
       QHash<QString, TeamData> m_teamCache;     // 队伍快速查找
       QMap<QString, QSet<QString>> m_problemCache; // 题目解决缓存
   };
   ```

2. **分页加载**：
   ```cpp
   class PaginatedModel {
       QList<TeamData> m_visibleTeams;  // 当前页数据
       int m_pageSize;
       int m_currentPage;
   };
   ```

3. **数据库集成**：
   - 考虑使用SQLite进行本地缓存
   - 支持更复杂的查询和索引

---

**文档版本：** v1.0  
**分析基准：** RankingSystem v1.0  
**最后更新：** 2025年6月30日
