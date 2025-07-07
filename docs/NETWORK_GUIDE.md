# 网络功能使用指南

## 概述

竞赛排行榜系统现已支持通过网络实时获取竞赛数据，告别了仅从本地文件读取的限制，实现了真正的实时性展示。

## 核心功能特性

### 🌐 多种数据源支持

1. **本地文件模式** - 传统的本地JSON文件读取
2. **网络实时模式** - 从竞赛服务器实时获取数据
3. **混合模式** - 网络优先，本地备份的智能切换

### 📡 网络协议支持

- **HTTP/HTTPS REST API** - 标准的Web API接口
- **DOMjudge API** - 兼容DOMjudge竞赛平台
- **ICPC Tools格式** - 支持ICPC标准数据格式
- **自定义协议** - 可扩展的数据格式支持

### 🔐 认证机制

- **无认证** - 开放的API接口
- **基本认证** - 用户名/密码验证
- **API密钥** - Bearer Token认证
- **SSL/TLS** - 安全传输层支持

## 使用步骤

### 1. 启动测试服务器

```bash
# 在项目根目录下
cd scripts
python3 test_server.py

# 自定义端口
python3 test_server.py 9090
```

服务器将在 `http://localhost:8080` 启动，提供以下API端点：
- `/api/teams` - 获取所有队伍数据
- `/api/contest` - 获取竞赛信息
- `/api/problems` - 获取题目列表
- `/api/ping` - 连接测试

### 2. 配置网络连接

1. **打开网络配置对话框**
   - 点击工具栏中的"网络配置"按钮
   - 或使用菜单: 工具 → 网络配置

2. **服务器配置**
   ```
   服务器地址: http://localhost:8080
   端口: 8080
   API端点: /api/teams
   数据源类型: HTTP REST API
   ```

3. **认证配置**（可选）
   ```
   认证方式: 无认证
   ```

4. **连接配置**
   ```
   更新间隔: 5000 ms
   启用自动重连: ✓
   最大重试次数: 3
   重试间隔: 5 秒
   ```

5. **测试连接**
   - 点击"测试连接"按钮验证配置
   - 查看连接状态和网络延迟

### 3. 切换数据源

在主界面工具栏中选择数据源：
- **本地文件** - 读取data目录下的JSON文件
- **网络实时** - 从配置的服务器获取数据
- **混合模式** - 网络优先，失败时自动切换到本地

### 4. 实时监控

- **网络状态指示器** - 显示当前连接状态和延迟
- **自动重连** - 网络断开时自动尝试重连
- **错误提示** - 网络异常时的友好提示

## API数据格式

### 队伍数据格式 (GET /api/teams)

```json
{
  "teams": [
    {
      "id": "team01",
      "name": "清华大学",
      "score": 785,
      "solved": 6,
      "lastSubmissionTime": "2024-07-07T14:30:15Z",
      "problems": {
        "A": {
          "attempts": 1,
          "solved": true,
          "firstSolved": "2024-07-07T09:15:30Z",
          "penalty": 0
        },
        "B": {
          "attempts": 3,
          "solved": true,
          "firstSolved": "2024-07-07T10:45:20Z",
          "penalty": 40
        }
      }
    }
  ],
  "timestamp": "2024-07-07T14:35:00Z",
  "totalTeams": 15
}
```

### 竞赛信息格式 (GET /api/contest)

```json
{
  "name": "2024年编程竞赛",
  "startTime": "2024-07-07T09:00:00Z",
  "duration": "05:00:00",
  "freezeTime": "04:00:00"
}
```

## 技术架构

### 网络层组件

```
NetworkManager (网络管理器)
├── QNetworkAccessManager - HTTP请求处理
├── 连接状态管理 - 自动重连机制
├── 数据解析器 - 多格式支持
└── 错误处理 - 优雅降级

DataManager (数据管理器)
├── 数据源切换 - 本地/网络/混合
├── 缓存机制 - 减少网络请求
├── 审计日志 - 操作记录
└── 查询树维护 - 实时更新索引
```

### 信号槽通信

```cpp
// 网络状态变化
NetworkManager::connected() → MainWindow::onNetworkConnected()
NetworkManager::networkError() → MainWindow::onNetworkError()
NetworkManager::teamDataReceived() → DataManager::onNetworkDataReceived()

// 数据源切换
DataManager::dataSourceChanged() → MainWindow::updateUI()
DataManager::networkConnected() → MainWindow::updateNetworkStatus()
```

## 配置文件

系统会自动保存网络配置到用户配置文件：

```ini
[network]
serverUrl=http://localhost:8080
serverPort=8080
apiEndpoint=/api/teams
dataSource=0
authType=0
updateInterval=5000
autoReconnect=true
maxRetries=3
retryInterval=5
```

## 故障排除

### 常见问题

1. **连接超时**
   - 检查服务器地址和端口
   - 确认防火墙设置
   - 验证网络连接

2. **认证失败**
   - 检查用户名/密码
   - 验证API密钥格式
   - 确认服务器认证配置

3. **数据格式错误**
   - 检查API端点路径
   - 验证服务器响应格式
   - 查看错误日志

### 调试模式

启用详细日志输出：
```cpp
// 在main.cpp中添加
QLoggingCategory::setFilterRules("*.debug=true");
```

### 网络诊断

使用内置的网络诊断工具：
1. 打开网络配置对话框
2. 查看连接状态和延迟信息
3. 检查最后更新时间和接收字节数

## 性能优化

### 带宽优化
- 数据压缩传输
- 增量更新支持
- 缓存机制减少请求

### 延迟优化
- 连接池复用
- 并发请求处理
- 本地缓存策略

### 可扩展性
- 模块化设计
- 插件式数据源
- 配置驱动架构

## 安全考虑

### 数据传输
- HTTPS强制加密
- 证书验证
- 请求头部认证

### 输入验证
- JSON格式验证
- 数据范围检查
- SQL注入防护

### 错误处理
- 优雅降级
- 敏感信息过滤
- 异常恢复机制

---

通过这套完整的网络功能，竞赛排行榜系统真正实现了：
- ⚡ **实时性** - 秒级数据更新
- 🔄 **可靠性** - 自动重连和故障转移
- 🛡️ **安全性** - 多重认证和加密传输
- 🔧 **可维护性** - 模块化设计和配置管理

让您的竞赛排行榜展示更加专业和现代！
