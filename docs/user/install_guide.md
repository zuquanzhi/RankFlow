# RankingSystem 安装指南

## 系统要求

### 最低配置

- **操作系统**：
  - Windows 10 或更新版本
  - macOS 10.14 或更新版本  
  - Ubuntu 18.04 或更新版本（Linux）
- **内存**：4GB RAM（推荐8GB）
- **磁盘空间**：至少200MB可用空间
- **其他**：支持OpenGL 2.0的显卡

### 推荐配置

- **内存**：8GB RAM或更多
- **分辨率**：1920x1080或更高
- **网络**：用于数据更新（可选）

## 🖥️ Windows 安装

### 方式一：安装程序（推荐）

1. **下载**：`RankingSystem-v1.0-Windows-Setup.exe`
2. **安装**：
   - 双击安装程序
   - 按照向导完成安装
   - 选择安装目录（默认：`C:\Program Files\RankingSystem`）
3. **启动**：从开始菜单或桌面快捷方式启动

### 方式二：便携版

1. **下载**：`RankingSystem-v1.0-Windows.zip`
2. **解压**：解压到任意目录
3. **启动**：双击 `RankingSystem.exe`

## 🍎 macOS 安装

### 标准安装（推荐）

1. **下载**：`RankingSystem-v1.0-Fixed-macOS.dmg`
2. **挂载**：双击DMG文件
3. **安装**：将 `RankingSystem.app` 拖拽到 `Applications` 文件夹
4. **启动**：
   - 打开 `Applications` 文件夹
   - **右键点击** `RankingSystem.app`
   - 选择 `打开`
   - 在安全警告中点击 `打开`

### 安全设置

**为什么需要右键打开？**

- macOS有严格的安全策略，防止运行未经验证的应用
- 右键打开是苹果官方推荐的安全方式
- 首次运行后，后续可以正常双击启动

**如果仍然无法打开：**

打开终端并执行：

```bash
sudo xattr -r -d com.apple.quarantine /Applications/RankingSystem.app
```

### 故障排除

如果遇到"意外退出"问题：

1. **使用修复版DMG**：确保下载 `RankingSystem-v1.0-Fixed-macOS.dmg`
2. **运行修复脚本**：
   ```bash
   cd /path/to/project
   ./fix_macos_app.sh
   ```
3. **查看详细故障排除**：参考 [macOS问题解决](../troubleshooting/macos_issues.md)

## 🐧 Linux 安装

### Ubuntu/Debian

1. **下载**：`RankingSystem-v1.0-Linux.tar.gz`
2. **解压**：
   ```bash
   tar -xzf RankingSystem-v1.0-Linux.tar.gz
   cd RankingSystem
   ```
3. **安装依赖**：
   ```bash
   sudo apt update
   sudo apt install qt5-default libqt5widgets5 libqt5gui5 libqt5core5a
   ```
4. **启动**：
   ```bash
   ./RankingSystem
   ```

### CentOS/RHEL

1. **解压文件**（同上）
2. **安装依赖**：
   ```bash
   sudo yum install qt5-qtbase qt5-qtbase-gui
   ```
3. **启动应用**（同上）

### 创建桌面快捷方式

```bash
cat > ~/.local/share/applications/rankingsystem.desktop << EOF
[Desktop Entry]
Name=RankingSystem
Comment=Competition Ranking System
Exec=/path/to/RankingSystem/RankingSystem
Icon=/path/to/RankingSystem/icon.png
Terminal=false
Type=Application
Categories=Office;Education;
EOF
```

## 📁 安装后配置

### 数据目录

安装后，应用会在以下位置创建数据目录：

- **Windows**：`%APPDATA%\RankingSystem`
- **macOS**：`~/Library/Application Support/RankingSystem`
- **Linux**：`~/.local/share/RankingSystem`

### 初始配置

1. **数据文件**：将比赛数据放入 `data` 目录
2. **日志文件**：查看 `logs` 目录了解运行状态
3. **配置文件**：编辑 `config.json` 进行个性化设置

## 🚀 验证安装

### 启动测试

1. **启动应用**：按照上述步骤启动
2. **加载示例数据**：使用内置示例数据测试
3. **基本功能**：测试排行榜显示、刷新功能
4. **数据导入**：尝试导入自己的数据文件

### 性能检查

- 内存使用应在100-200MB范围内
- CPU使用率在空闲时应低于5%
- 界面响应应该流畅无卡顿

## 🔧 卸载指南

### Windows

- **安装版**：通过"程序和功能"卸载
- **便携版**：直接删除文件夹

### macOS

1. 将应用从 `Applications` 文件夹拖到废纸篓
2. 删除用户数据：`~/Library/Application Support/RankingSystem`

### Linux

1. 删除应用文件夹
2. 删除配置文件：`~/.local/share/RankingSystem`
3. 删除桌面快捷方式：`~/.local/share/applications/rankingsystem.desktop`

## 📞 技术支持

### 安装问题

如果遇到安装问题，请：

1. 检查系统要求是否满足
2. 确认下载文件完整性
3. 查看相关平台的故障排除文档
4. 联系技术支持

### 相关文档

- [用户手册](user_manual.md)
- [数据导入指南](data_import_guide.md)
- [故障排除](../troubleshooting/common_issues.md)
- [macOS专项问题](../troubleshooting/macos_issues.md)

---

**版本信息：** v1.0  
**最后更新：** 2025年6月30日  
**技术支持：** 开发团队
