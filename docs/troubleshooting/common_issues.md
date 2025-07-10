# RankingSystem 常见问题解答

## 概述

本文档汇总了 RankingSystem 使用过程中的常见问题和解决方案，适用于所有平台的用户。

## 🚀 安装和启动问题

### Q1: 应用无法启动

**问题描述**：双击应用图标后没有反应，或者显示错误信息。

**可能原因**：
- 缺少运行依赖
- 权限不足
- 系统兼容性问题

**解决方案**：

#### Windows
```cmd
# 检查Visual C++运行库
# 下载并安装 Microsoft Visual C++ Redistributable

# 以管理员身份运行
右键点击应用 → "以管理员身份运行"
```

#### macOS
```bash
# 移除隔离属性
sudo xattr -r -d com.apple.quarantine /Applications/RankingSystem.app

# 右键打开（首次运行推荐）
右键点击应用 → 选择"打开"
```

#### Linux
```bash
# 安装Qt5依赖
sudo apt install qt5-default libqt5widgets5 libqt5charts5

# 检查可执行权限
chmod +x RankingSystem
```

### Q2: 提示"文件已损坏"或"无法验证"

**适用平台**：macOS

**解决方案**：
1. 重新下载安装包
2. 检查下载完整性：
   ```bash
   shasum -a 256 RankingSystem-*.dmg
   ```
3. 移除隔离属性：
   ```bash
   xattr -d com.apple.quarantine RankingSystem-*.dmg
   ```

### Q3: 安装后找不到应用

**解决方案**：

#### Windows
- 检查开始菜单中的程序列表
- 查看桌面快捷方式
- 搜索"RankingSystem"

#### macOS
- 在 Applications 文件夹中查找
- 使用 Spotlight 搜索
- 检查 Launchpad

#### Linux
- 查看应用程序菜单
- 使用命令行启动：`./RankingSystem`

## 📊 数据相关问题

### Q4: 排行榜显示为空

**问题描述**：应用启动后排行榜表格没有数据。

**解决方案**：
1. **检查数据文件**：
   ```bash
   # 确保数据目录存在
   ls -la data/
   
   # 检查JSON文件格式
   python -m json.tool data/team01.json
   ```

2. **验证文件权限**：
   ```bash
   chmod 644 data/*.json
   ```

3. **手动刷新数据**：
   - 点击工具栏的"手动刷新"按钮
   - 检查状态栏是否显示加载信息

### Q5: 数据更新不及时

**问题描述**：修改数据文件后，应用没有自动更新。

**解决方案**：
1. **启用自动刷新**：
   - 勾选"自动刷新"选项
   - 设置合适的刷新间隔（推荐60-300秒）

2. **检查文件监控**：
   - 确保数据文件在正确路径
   - 验证文件修改时间

3. **手动触发更新**：
   - 使用"手动刷新"按钮
   - 重启应用程序

### Q6: 数据格式错误

**问题描述**：导入数据时提示格式错误。

**解决方案**：
1. **验证JSON格式**：
   ```json
   {
     "teamName": "示例队伍",
     "totalScore": 100,
     "solvedProblems": 3,
     "accuracy": 85.5,
     "lastSubmission": "2025-06-30T10:30:00"
   }
   ```

2. **检查必需字段**：
   - `teamName`：队伍名称
   - `totalScore`：总分
   - `solvedProblems`：解决题目数

3. **使用在线JSON验证器**：
   - 复制JSON内容到验证工具
   - 修复语法错误

## 🖥️ 界面和显示问题

### Q7: 界面显示不完整

**问题描述**：部分界面元素缺失或显示异常。

**解决方案**：
1. **调整窗口大小**：
   - 拖拽窗口边框
   - 使用全屏模式（F11）

2. **检查分辨率**：
   - 最低分辨率：1024x768
   - 推荐分辨率：1920x1080

3. **重置界面布局**：
   - 删除配置文件
   - 重启应用程序

### Q8: 字体显示问题

**问题描述**：文字显示模糊或字体异常。

**解决方案**：
1. **Windows**：
   - 调整显示缩放设置
   - 更新显卡驱动

2. **macOS**：
   - 检查"显示器"设置
   - 调整字体平滑选项

3. **Linux**：
   - 安装字体包：`sudo apt install fonts-dejavu`
   - 配置字体渲染

### Q9: 图表不显示

**问题描述**：数据可视化图表区域为空。

**解决方案**：
1. **检查数据**：
   - 确保有足够的数据用于绘图
   - 验证数据格式正确

2. **切换图表类型**：
   - 尝试不同的图表类型
   - 检查图表配置选项

3. **重新加载数据**：
   - 点击刷新按钮
   - 重启应用程序

## ⚡ 性能问题

### Q10: 应用运行缓慢

**问题描述**：应用响应慢，操作卡顿。

**解决方案**：
1. **调整刷新设置**：
   - 增加刷新间隔（建议300秒以上）
   - 减少同时显示的队伍数量

2. **关闭动画效果**：
   - 禁用图表动画
   - 减少视觉效果

3. **系统优化**：
   - 关闭其他占用资源的程序
   - 确保足够的可用内存（推荐4GB+）

### Q11: 内存使用过高

**问题描述**：应用占用内存过多。

**解决方案**：
1. **检查数据量**：
   - 限制队伍数量（建议<100）
   - 清理历史数据

2. **重启应用**：
   - 定期重启应用释放内存
   - 检查内存泄漏

3. **系统监控**：
   ```bash
   # Linux/macOS
   top -p $(pgrep RankingSystem)
   
   # Windows
   # 使用任务管理器查看
   ```

## 🔧 配置问题

### Q12: 设置无法保存

**问题描述**：应用设置在重启后丢失。

**解决方案**：
1. **检查配置文件权限**：
   ```bash
   # 配置文件位置
   # Windows: %APPDATA%\RankingSystem\
   # macOS: ~/Library/Application Support/RankingSystem/
   # Linux: ~/.config/RankingSystem/
   
   chmod 644 config.json
   ```

2. **手动创建配置**：
   ```json
   {
     "autoRefresh": true,
     "refreshInterval": 60,
     "theme": "standard",
     "sortOrder": "totalScore"
   }
   ```

### Q13: 主题设置异常

**问题描述**：界面主题显示不正确。

**解决方案**：
1. **重置主题设置**：
   - 删除主题配置文件
   - 选择标准主题

2. **检查系统主题**：
   - 确保系统主题兼容
   - 尝试不同主题选项

## 🌐 网络相关问题

### Q14: API数据无法加载

**问题描述**：使用网络数据源时无法获取数据。

**解决方案**：
1. **检查网络连接**：
   ```bash
   ping api.example.com
   curl -I https://api.example.com/rankings
   ```

2. **验证API设置**：
   - 检查API地址是否正确
   - 验证认证信息
   - 确认API接口可用性

3. **代理设置**：
   - 配置代理服务器（如需要）
   - 检查防火墙设置

### Q15: 证书验证失败

**问题描述**：HTTPS连接时提示证书错误。

**解决方案**：
1. **更新系统时间**：
   - 确保系统时间正确
   - 同步网络时间

2. **更新证书**：
   - 更新系统根证书
   - 检查中间证书

## 🛠️ 故障排除工具

### 诊断命令

```bash
# 系统信息
uname -a                    # Linux/macOS
systeminfo                 # Windows

# 应用进程
ps aux | grep RankingSystem # Linux/macOS
tasklist | findstr Ranking # Windows

# 网络连接
netstat -an | grep :80     # Linux/macOS
netstat -an | findstr :80  # Windows

# 磁盘空间
df -h                      # Linux/macOS
dir /-s                    # Windows
```

### 日志查看

```bash
# 应用日志位置
# Windows: %APPDATA%\RankingSystem\logs\
# macOS: ~/Library/Application Support/RankingSystem/logs/
# Linux: ~/.local/share/RankingSystem/logs/

tail -f application.log
```

### 重置应用

```bash
# 完全重置应用设置
rm -rf ~/.config/RankingSystem/     # Linux
rm -rf ~/Library/Application\ Support/RankingSystem/  # macOS
rmdir /s "%APPDATA%\RankingSystem"  # Windows
```

## 📞 获取帮助

### 问题报告

如果以上解决方案都无法解决您的问题，请提供以下信息：

1. **系统信息**：
   - 操作系统版本
   - 应用程序版本
   - 硬件配置

2. **问题描述**：
   - 具体错误信息
   - 重现步骤
   - 发生频率

3. **日志文件**：
   - 应用程序日志
   - 系统崩溃日志
   - 错误截图

### 联系方式

- **技术支持邮箱**：support@example.com
- **问题反馈**：GitHub Issues
- **用户社区**：Discord/QQ群

### 相关文档

- [用户手册](../user/user_manual.md)
- [安装指南](../user/install_guide.md)
- [技术文档](../technical/architecture_integrated.md)
- [macOS专项问题](macos_issues.md)

---

**FAQ版本：** v1.0  
**最后更新：** 2025年6月30日  
**维护团队：** 技术支持组
