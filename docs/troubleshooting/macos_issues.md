# macOS 问题排除指南

## 概述

本文档专门针对 macOS 平台下 RankingSystem 的安装和运行问题提供详细的解决方案。

## 常见问题分类

### 🚫 安装问题

#### 问题1：DMG文件无法打开

**症状**：
- 双击DMG文件没有反应
- 提示"文件已损坏"
- 磁盘映像验证失败

**解决方案**：

1. **检查下载完整性**
   ```bash
   # 检查文件大小（应该约为50-100MB）
   ls -lh RankingSystem-*.dmg
   
   # 重新下载文件
   curl -L -O <download_url>
   ```

2. **移除隔离属性**
   ```bash
   xattr -d com.apple.quarantine RankingSystem-v1.0-Fixed-macOS.dmg
   ```

3. **强制挂载DMG**
   ```bash
   hdiutil attach RankingSystem-v1.0-Fixed-macOS.dmg -readonly
   ```

#### 问题2：应用拖拽失败

**症状**：
- 无法将应用拖拽到Applications文件夹
- 提示权限不足

**解决方案**：

1. **检查磁盘空间**
   ```bash
   df -h /Applications
   ```

2. **修复权限**
   ```bash
   sudo chown -R $(whoami):staff /Applications
   ```

3. **手动复制**
   ```bash
   cp -R "/Volumes/RankingSystem/RankingSystem.app" /Applications/
   ```

### ⚡ 启动问题

#### 问题1："RankingSystem"意外退出

**症状**：
- 应用启动后立即崩溃
- 显示"意外退出"对话框
- 在活动监视器中看到应用进程消失

**根本原因**：
- macOS 15.5+ 对代码签名要求更严格
- adhoc签名与依赖库签名不一致
- 系统安全策略阻止运行

**解决方案步骤**：

1. **使用修复版DMG**
   ```bash
   # 确保使用修复版DMG
   # RankingSystem-v1.0-Fixed-macOS.dmg
   ```

2. **移除隔离属性**
   ```bash
   sudo xattr -r -d com.apple.quarantine /Applications/RankingSystem.app
   ```

3. **运行修复脚本**
   ```bash
   # 下载并运行修复脚本
   curl -O <script_url>/fix_macos_app.sh
   chmod +x fix_macos_app.sh
   ./fix_macos_app.sh /Applications/RankingSystem.app
   ```

4. **验证修复**
   ```bash
   # 检查签名状态
   codesign -v /Applications/RankingSystem.app
   
   # 检查依赖
   otool -L /Applications/RankingSystem.app/Contents/MacOS/RankingSystem
   ```

#### 问题2：Gatekeeper阻止

**症状**：
- 提示"无法打开，因为它来自未经认证的开发者"
- 系统偏好设置显示被阻止的应用

**解决方案**：

1. **通过系统偏好设置允许**
   - 打开"系统偏好设置" → "安全性与隐私" → "通用"
   - 查找被阻止的应用提示
   - 点击"仍要打开"或"允许"

2. **右键打开方式**
   - 右键点击应用图标
   - 选择"打开"
   - 在警告对话框中点击"打开"

3. **命令行绕过**
   ```bash
   spctl --add /Applications/RankingSystem.app
   ```

#### 问题3：权限不足

**症状**：
- 应用无法读取/写入数据文件
- 无法创建日志文件
- 功能异常

**解决方案**：

1. **检查应用权限**
   ```bash
   ls -la /Applications/RankingSystem.app
   ```

2. **修复权限**
   ```bash
   sudo chmod -R 755 /Applications/RankingSystem.app
   sudo chown -R $(whoami):staff /Applications/RankingSystem.app
   ```

3. **数据目录权限**
   ```bash
   mkdir -p ~/Library/Application\ Support/RankingSystem
   chmod 755 ~/Library/Application\ Support/RankingSystem
   ```

### 🖥️ 运行时问题

#### 问题1：界面显示异常

**症状**：
- 界面元素缺失或错位
- 字体显示问题
- 颜色异常

**解决方案**：

1. **检查系统兼容性**
   ```bash
   sw_vers  # 查看macOS版本
   ```

2. **重置显示设置**
   - 删除应用偏好设置文件
   - 重启应用

3. **调整系统设置**
   - 检查"显示器"设置中的分辨率和缩放
   - 调整"辅助功能"中的显示选项

#### 问题2：数据加载失败

**症状**：
- 排行榜显示为空
- 数据文件无法读取
- 提示文件权限错误

**解决方案**：

1. **检查数据文件路径**
   ```bash
   find /Applications/RankingSystem.app -name "*.json" -type f
   ```

2. **验证数据格式**
   ```bash
   python -m json.tool /path/to/data.json
   ```

3. **修复数据权限**
   ```bash
   chmod 644 /Applications/RankingSystem.app/Contents/MacOS/data/*.json
   ```

#### 问题3：性能问题

**症状**：
- 应用运行缓慢
- 内存使用过高
- CPU占用率高

**解决方案**：

1. **监控资源使用**
   ```bash
   # 使用活动监视器检查
   top -pid $(pgrep RankingSystem)
   ```

2. **调整应用设置**
   - 降低刷新频率
   - 关闭不必要的动画
   - 减少显示的数据量

3. **系统优化**
   - 关闭其他不必要的应用
   - 释放磁盘空间
   - 重启系统

## 诊断工具

### 自动诊断脚本

```bash
#!/bin/bash
# diagnose_macos_issues.sh

echo "🔍 macOS RankingSystem 诊断工具"
echo "================================"

APP_PATH="/Applications/RankingSystem.app"

# 检查应用是否存在
if [ ! -d "$APP_PATH" ]; then
    echo "❌ 应用未安装在 /Applications/"
    exit 1
fi

echo "✅ 应用已安装"

# 检查签名状态
echo "🔒 检查代码签名..."
if codesign -v "$APP_PATH" 2>/dev/null; then
    echo "✅ 代码签名有效"
else
    echo "⚠️ 代码签名问题"
fi

# 检查隔离属性
echo "🔐 检查隔离属性..."
if xattr "$APP_PATH" | grep -q "com.apple.quarantine"; then
    echo "⚠️ 应用被隔离"
    echo "执行: sudo xattr -r -d com.apple.quarantine '$APP_PATH'"
else
    echo "✅ 无隔离属性"
fi

# 检查权限
echo "📋 检查文件权限..."
PERMS=$(stat -f "%p" "$APP_PATH")
if [ "$PERMS" -ge 755 ]; then
    echo "✅ 权限正常"
else
    echo "⚠️ 权限不足"
    echo "执行: chmod 755 '$APP_PATH'"
fi

# 检查依赖
echo "🔗 检查依赖库..."
EXECUTABLE="$APP_PATH/Contents/MacOS/RankingSystem"
if [ -f "$EXECUTABLE" ]; then
    MISSING_DEPS=$(otool -L "$EXECUTABLE" | grep -v ":" | grep -v "@" | grep "/" | while read dep; do
        if [ ! -f "$dep" ]; then
            echo "$dep"
        fi
    done)
    
    if [ -z "$MISSING_DEPS" ]; then
        echo "✅ 所有依赖库存在"
    else
        echo "⚠️ 缺失依赖库:"
        echo "$MISSING_DEPS"
    fi
else
    echo "❌ 可执行文件不存在"
fi

# 检查崩溃日志
echo "📄 检查崩溃日志..."
CRASH_LOGS=$(ls ~/Library/Logs/DiagnosticReports/RankingSystem* 2>/dev/null | head -5)
if [ -n "$CRASH_LOGS" ]; then
    echo "⚠️ 发现崩溃日志:"
    echo "$CRASH_LOGS"
else
    echo "✅ 无崩溃日志"
fi

echo ""
echo "🎯 诊断完成"
```

### 修复工具

```bash
#!/bin/bash
# fix_macos_app.sh

APP_PATH="${1:-/Applications/RankingSystem.app}"

echo "🛠️ 修复 macOS RankingSystem 应用"
echo "================================="
echo "目标应用: $APP_PATH"

if [ ! -d "$APP_PATH" ]; then
    echo "❌ 应用路径不存在: $APP_PATH"
    exit 1
fi

# 1. 移除隔离属性
echo "🔐 移除隔离属性..."
sudo xattr -r -d com.apple.quarantine "$APP_PATH" 2>/dev/null
echo "✅ 隔离属性已移除"

# 2. 修复权限
echo "📋 修复文件权限..."
sudo chmod -R 755 "$APP_PATH"
sudo chown -R $(whoami):staff "$APP_PATH"
echo "✅ 权限已修复"

# 3. 重新签名
echo "🔒 重新签名应用..."
codesign --force --deep --sign - "$APP_PATH"
echo "✅ 应用已重新签名"

# 4. 验证修复
echo "🔍 验证修复结果..."
if codesign -v "$APP_PATH" 2>/dev/null; then
    echo "✅ 签名验证通过"
else
    echo "⚠️ 签名验证失败"
fi

# 5. 尝试启动
echo "🚀 尝试启动应用..."
open "$APP_PATH" &
LAUNCH_PID=$!

sleep 3

if kill -0 $LAUNCH_PID 2>/dev/null; then
    echo "✅ 应用启动成功"
else
    echo "❌ 应用启动失败"
    echo "请查看控制台日志获取更多信息"
fi

echo ""
echo "🎯 修复完成"
```

## 预防措施

### 开发者建议

1. **使用官方开发者证书**
   - 申请Apple开发者账号
   - 使用Developer ID证书签名
   - 进行应用公证

2. **改进打包流程**
   - 统一签名所有依赖
   - 完善应用信息
   - 添加启动脚本

3. **提供多种分发方式**
   - 提供Homebrew安装选项
   - 创建PKG安装包
   - 支持源码编译

### 用户建议

1. **下载来源**
   - 从官方网站下载
   - 验证文件完整性
   - 避免第三方修改版本

2. **安装最佳实践**
   - 右键打开应用
   - 及时更新macOS
   - 保持系统安全设置适中

3. **问题反馈**
   - 记录详细错误信息
   - 提供系统版本信息
   - 上传崩溃日志

## 相关资源

### 官方文档

- [Apple代码签名指南](https://developer.apple.com/documentation/security/code_signing_services)
- [macOS应用分发](https://developer.apple.com/documentation/xcode/distributing_your_app_outside_the_mac_app_store)
- [Gatekeeper说明](https://support.apple.com/HT202491)

### 社区资源

- [Qt macOS部署指南](https://doc.qt.io/qt-5/macos-deployment.html)
- [macOS打包最佳实践](https://stackoverflow.com/questions/tagged/macos+code-signing)

---

**文档版本：** v1.0  
**适用系统：** macOS 10.14+  
**最后更新：** 2025年6月30日  
**问题反馈：** 技术支持团队
