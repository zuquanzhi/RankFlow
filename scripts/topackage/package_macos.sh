#!/bin/bash

echo "📦 竞赛排行榜系统 - macOS 打包脚本"
echo "======================================"

# 检查依赖
echo "🔍 检查打包依赖..."

# 查找 macdeployqt 工具
MACDEPLOYQT=""
if command -v macdeployqt &> /dev/null; then
    MACDEPLOYQT="macdeployqt"
elif [ -f "/opt/homebrew/bin/macdeployqt" ]; then
    MACDEPLOYQT="/opt/homebrew/bin/macdeployqt"
else
    # 搜索 Homebrew Cellar 目录
    MACDEPLOYQT_SEARCH=$(find /opt/homebrew/Cellar/qt@5/*/bin/macdeployqt 2>/dev/null | head -n1)
    if [ -n "$MACDEPLOYQT_SEARCH" ]; then
        MACDEPLOYQT="$MACDEPLOYQT_SEARCH"
    else
        # 尝试 /usr/local 路径
        MACDEPLOYQT_SEARCH=$(find /usr/local/Cellar/qt@5/*/bin/macdeployqt 2>/dev/null | head -n1)
        if [ -n "$MACDEPLOYQT_SEARCH" ]; then
            MACDEPLOYQT="$MACDEPLOYQT_SEARCH"
        elif [ -f "/usr/local/bin/macdeployqt" ]; then
            MACDEPLOYQT="/usr/local/bin/macdeployqt"
        fi
    fi
fi

if [ -z "$MACDEPLOYQT" ]; then
    echo "❌ 未找到 macdeployqt"
    echo "请确保 Qt5 正确安装: brew install qt@5"
    exit 1
fi

echo "✅ 找到 macdeployqt: $MACDEPLOYQT"

# 检查可执行文件
if [ ! -f "build/bin/RankingSystem" ]; then
    echo "❌ 未找到可执行文件"
    echo "请先编译项目: ./build.sh"
    exit 1
fi

echo "✅ 依赖检查完成"

# 创建打包目录
PACKAGE_DIR="package"
APP_NAME="RankingSystem.app"
APP_DIR="$PACKAGE_DIR/$APP_NAME"

echo "📁 创建应用包结构..."
rm -rf "$PACKAGE_DIR"
mkdir -p "$APP_DIR/Contents/"{MacOS,Resources,Frameworks}

# 复制可执行文件
echo "📋 复制可执行文件..."
cp "build/bin/RankingSystem" "$APP_DIR/Contents/MacOS/"

# 创建 Info.plist
echo "📝 创建 Info.plist..."
cat > "$APP_DIR/Contents/Info.plist" << 'EOF'
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleExecutable</key>
    <string>RankingSystem</string>
    <key>CFBundleIdentifier</key>
    <string>com.contest.rankingsystem</string>
    <key>CFBundleName</key>
    <string>竞赛排行榜系统</string>
    <key>CFBundleDisplayName</key>
    <string>竞赛排行榜系统</string>
    <key>CFBundleVersion</key>
    <string>1.0.0</string>
    <key>CFBundleShortVersionString</key>
    <string>1.0</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>CFBundleSignature</key>
    <string>RANK</string>
    <key>CFBundleIconFile</key>
    <string>app_icon</string>
    <key>LSMinimumSystemVersion</key>
    <string>10.14</string>
    <key>NSPrincipalClass</key>
    <string>NSApplication</string>
    <key>NSHighResolutionCapable</key>
    <true/>
</dict>
</plist>
EOF

# 复制应用图标
echo "🎨 复制应用图标..."
if [ -f "resources/icons/app_icon.icns" ]; then
    cp "resources/icons/app_icon.icns" "$APP_DIR/Contents/Resources/"
    echo "✅ 图标复制成功"
else
    echo "⚠️ 未找到应用图标文件"
fi

# 使用 macdeployqt 处理依赖
echo "🔗 处理 Qt 依赖..."
"$MACDEPLOYQT" "$APP_DIR" -verbose=2

if [ $? -ne 0 ]; then
    echo "❌ macdeployqt 失败"
    exit 1
fi

# 尝试重新签名（如果有开发者证书）
echo "✍️ 处理代码签名..."
if security find-identity -v -p codesigning | grep -q "Developer ID Application"; then
    echo "🔐 找到开发者证书，进行签名..."
    codesign --force --sign "Developer ID Application" --deep "$APP_DIR"
    if [ $? -eq 0 ]; then
        echo "✅ 代码签名成功"
    else
        echo "⚠️ 代码签名失败，继续使用adhoc签名"
    fi
else
    echo "⚠️ 未找到开发者证书，使用adhoc签名"
    echo "   用户首次运行时需要手动允许"
fi

# 复制数据文件和文档
echo "📂 复制数据文件和文档..."
cp -r data "$APP_DIR/Contents/MacOS/"
cp -r docs "$APP_DIR/Contents/MacOS/"
cp README.md "$APP_DIR/Contents/MacOS/"
cp LICENSE "$APP_DIR/Contents/MacOS/"

# 创建启动脚本
echo "📜 创建启动脚本..."
cat > "$APP_DIR/Contents/MacOS/start.sh" << 'EOF'
#!/bin/bash
cd "$(dirname "$0")"
./RankingSystem
EOF
chmod +x "$APP_DIR/Contents/MacOS/start.sh"

# 创建用户安装说明文件
echo "📝 创建安装说明..."
cat > "$PACKAGE_DIR/安装说明.txt" << 'EOF'
🚀 竞赛排行榜系统安装指南

如果遇到"无法打开应用"的安全警告，请按以下步骤操作：

方法1：系统偏好设置
1. 尝试启动应用时会弹出警告
2. 打开"系统偏好设置" → "安全性与隐私" → "通用"
3. 点击"仍要打开"按钮

方法2：右键打开
1. 右键点击应用图标
2. 选择"打开"
3. 在确认对话框中点击"打开"

方法3：命令行（高级用户）
在终端中执行：
sudo xattr -r -d com.apple.quarantine /Applications/RankingSystem.app

技术支持：
- 本应用已包含所有必要依赖
- 支持 macOS 10.14 及更新版本
- 如有问题请查看项目文档
EOF

cat > "$PACKAGE_DIR/Installation Guide.txt" << 'EOF'
🚀 Ranking System Installation Guide

If you encounter "Cannot open app" security warning, follow these steps:

Method 1: System Preferences
1. Try to launch the app and see the warning
2. Open "System Preferences" → "Security & Privacy" → "General"
3. Click "Open Anyway" button

Method 2: Right-click to Open
1. Right-click on the app icon
2. Select "Open"
3. Click "Open" in the confirmation dialog

Method 3: Command Line (Advanced)
Execute in Terminal:
sudo xattr -r -d com.apple.quarantine /Applications/RankingSystem.app

Technical Support:
- This app includes all necessary dependencies
- Supports macOS 10.14 and later
- Check project documentation for more help
EOF

# 创建 DMG 文件
echo "💿 创建 DMG 安装包..."
DMG_NAME="RankingSystem-v1.0-macOS.dmg"
hdiutil create -volname "竞赛排行榜系统 v1.0" -srcfolder "$PACKAGE_DIR" -ov -format UDZO "$DMG_NAME"

if [ $? -eq 0 ]; then
    echo "✅ macOS 打包完成！"
    echo ""
    echo "📦 生成的文件:"
    echo "   应用包: $APP_DIR"
    echo "   安装包: $DMG_NAME"
    echo ""
    echo "🚀 使用方法:"
    echo "   1. 双击 $DMG_NAME 打开"
    echo "   2. 拖拽应用到 Applications 文件夹"
    echo "   3. 右键点击应用选择'打开'（首次运行）"
    echo ""
    echo "⚠️ 重要提示："
    echo "   - 首次运行可能会遇到安全警告"
    echo "   - 请查看DMG中的'安装说明.txt'文件"
    echo "   - 或右键点击应用选择'打开'来绕过警告"
    echo ""
    echo "📋 分发说明:"
    echo "   - DMG 文件可直接分发给其他 macOS 用户"
    echo "   - 支持 macOS 10.14+ 系统"
    echo "   - 包含所有依赖，无需额外安装"
else
    echo "❌ DMG 创建失败"
    exit 1
fi
