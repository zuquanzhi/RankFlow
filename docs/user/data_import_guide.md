# 数据导入指南

## 概述

RankingSystem 支持多种数据格式的导入，适用于不同的比赛场景和数据源。本指南将详细介绍各种数据格式和导入方法。

## 支持的数据格式

### 1. JSON 格式（推荐）

#### 队伍数据格式

```json
{
  "teamName": "队伍名称",
  "totalScore": 100,
  "solvedProblems": 3,
  "accuracy": 85.5,
  "submissions": [
    {
      "problemId": "A",
      "score": 50,
      "timestamp": "2025-06-30T10:30:00",
      "status": "AC"
    }
  ],
  "lastSubmission": "2025-06-30T10:30:00"
}
```

#### 字段说明

| 字段 | 类型 | 必需 | 说明 |
|------|------|------|------|
| `teamName` | String | ✅ | 队伍名称，唯一标识 |
| `totalScore` | Number | ✅ | 队伍总分 |
| `solvedProblems` | Number | ✅ | 解决的题目数量 |
| `accuracy` | Number | ❌ | 准确率（0-100） |
| `submissions` | Array | ❌ | 提交记录数组 |
| `lastSubmission` | String | ❌ | 最后提交时间（ISO格式） |

#### 提交记录格式

```json
{
  "problemId": "A",
  "score": 50,
  "timestamp": "2025-06-30T10:30:00",
  "status": "AC",
  "penalty": 0,
  "attempts": 1
}
```

| 字段 | 类型 | 必需 | 说明 |
|------|------|------|------|
| `problemId` | String | ✅ | 题目标识 |
| `score` | Number | ✅ | 获得分数 |
| `timestamp` | String | ✅ | 提交时间 |
| `status` | String | ✅ | 状态（AC/WA/TLE/CE等） |
| `penalty` | Number | ❌ | 罚时（分钟） |
| `attempts` | Number | ❌ | 尝试次数 |

### 2. CSV 格式

#### 基本格式

```csv
队伍名称,总分,通过题数,准确率,最后提交时间
冠军队伍,300,5,90.5,2025-06-30T10:30:00
算法大师,250,4,88.2,2025-06-30T10:25:00
```

#### 扩展格式（包含提交详情）

```csv
队伍名称,总分,通过题数,准确率,题目A,题目B,题目C,最后提交
冠军队伍,300,5,90.5,100,100,100,2025-06-30T10:30:00
算法大师,250,4,88.2,100,100,50,2025-06-30T10:25:00
```

### 3. 实时 API 接口

#### 接口规范

```http
GET /api/rankings
Content-Type: application/json

Response:
{
  "teams": [
    {
      "teamName": "队伍名称",
      "totalScore": 100,
      "solvedProblems": 3,
      "accuracy": 85.5,
      "lastSubmission": "2025-06-30T10:30:00"
    }
  ],
  "lastUpdate": "2025-06-30T10:30:00",
  "contestInfo": {
    "name": "比赛名称",
    "problems": ["A", "B", "C", "D", "E"]
  }
}
```

## 数据导入方法

### 1. 文件导入

#### 步骤

1. **准备数据文件**
   - 将数据文件放在 `data` 目录下
   - 支持格式：`.json`、`.csv`
   - 文件名格式：`team01.json`、`team02.json` 等

2. **手动导入**
   - 打开应用程序
   - 点击"手动刷新"按钮
   - 系统自动扫描并导入数据

3. **自动监控**
   - 启用"自动刷新"
   - 系统定期检查文件变化
   - 自动更新显示内容

#### 目录结构

```
data/
├── team01.json          # 队伍1数据
├── team02.json          # 队伍2数据
├── team03.json          # 队伍3数据
├── config.json          # 配置文件（可选）
└── problems.json        # 题目信息（可选）
```

### 2. 网络导入

#### 配置 API 接口

1. **编辑配置文件** `config.json`：

```json
{
  "dataSource": {
    "type": "api",
    "url": "https://your-contest-api.com/rankings",
    "interval": 60,
    "headers": {
      "Authorization": "Bearer your-token"
    }
  }
}
```

2. **重启应用程序**，系统自动使用 API 数据源

### 3. 数据库导入

#### 支持的数据库

- MySQL
- PostgreSQL
- SQLite

#### 配置示例

```json
{
  "dataSource": {
    "type": "database",
    "connection": {
      "host": "localhost",
      "port": 3306,
      "database": "contest",
      "username": "user",
      "password": "pass"
    },
    "queries": {
      "teams": "SELECT * FROM teams",
      "submissions": "SELECT * FROM submissions"
    }
  }
}
```

## 数据验证

### 自动验证

系统会自动验证以下内容：

- **格式检查**：JSON/CSV 格式是否正确
- **字段检查**：必需字段是否存在
- **数据类型**：字段类型是否匹配
- **数据完整性**：关联数据是否一致

### 验证结果

- ✅ **成功**：数据导入成功，显示在排行榜中
- ⚠️ **警告**：部分数据有问题，已忽略异常数据
- ❌ **失败**：数据格式错误，导入失败

### 错误处理

常见错误及解决方法：

| 错误类型 | 可能原因 | 解决方法 |
|----------|----------|----------|
| 格式错误 | JSON/CSV 语法错误 | 检查文件格式，使用验证工具 |
| 字段缺失 | 缺少必需字段 | 添加缺失的字段 |
| 类型错误 | 字段类型不匹配 | 修正字段类型 |
| 文件权限 | 无法读取文件 | 检查文件权限 |
| 网络错误 | API 接口无法访问 | 检查网络连接和接口状态 |

## 实时更新

### 文件监控

系统支持实时监控数据文件变化：

1. **启用自动刷新**
2. **设置监控间隔**（推荐30-60秒）
3. **修改数据文件**，系统自动检测并更新

### API 轮询

对于 API 数据源：

1. **配置轮询间隔**
2. **设置错误重试机制**
3. **启用缓存机制**防止频繁请求

## 性能优化

### 大数据量处理

- **分批加载**：大量数据分批次加载
- **增量更新**：只更新变化的数据
- **索引优化**：为常用查询字段建立索引

### 内存管理

- **数据缓存**：合理使用内存缓存
- **垃圾回收**：定期清理无用数据
- **资源限制**：设置内存使用上限

## 示例数据

### 完整示例

```json
{
  "teamName": "示例队伍",
  "totalScore": 285,
  "solvedProblems": 4,
  "accuracy": 87.5,
  "submissions": [
    {
      "problemId": "A",
      "score": 100,
      "timestamp": "2025-06-30T09:15:00",
      "status": "AC",
      "penalty": 0,
      "attempts": 1
    },
    {
      "problemId": "B",
      "score": 85,
      "timestamp": "2025-06-30T10:30:00",
      "status": "AC",
      "penalty": 20,
      "attempts": 3
    },
    {
      "problemId": "C",
      "score": 100,
      "timestamp": "2025-06-30T11:45:00",
      "status": "AC",
      "penalty": 0,
      "attempts": 1
    },
    {
      "problemId": "D",
      "score": 0,
      "timestamp": "2025-06-30T12:00:00",
      "status": "WA",
      "penalty": 20,
      "attempts": 2
    }
  ],
  "lastSubmission": "2025-06-30T12:00:00"
}
```

## 故障排除

### 常见问题

1. **数据不显示**
   - 检查文件路径和权限
   - 验证数据格式
   - 查看错误日志

2. **更新不及时**
   - 检查自动刷新设置
   - 验证文件监控功能
   - 调整刷新间隔

3. **性能问题**
   - 减少数据量
   - 优化刷新频率
   - 检查系统资源

### 调试工具

- **日志查看**：检查应用程序日志
- **数据验证**：使用内置验证工具
- **网络诊断**：测试 API 接口连通性

---

**版本信息：** v1.0  
**最后更新：** 2025年6月30日  
**相关文档：** [用户手册](user_manual.md) | [安装指南](install_guide.md)
