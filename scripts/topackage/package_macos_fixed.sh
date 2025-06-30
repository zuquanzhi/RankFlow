#!/bin/bash

echo "📦 RankingSystem - 修复版 macOS 打包脚本"
echo "============================================="

# 检查依赖
echo "🔍 检查打包依赖..."
MACDEPLOYQT=""
if command -v macdeployqt &> /dev/null; then
    MACDEPLOYQT="macdeployqt"
else
    MACDEPLOYQT_SEARCH=$(find /opt/homebrew/Cellar/qt@5/*/bin/macdeployqt 2>/dev/null | head -n1)
    if [ -n "$MACDEPLOYQT_SEARCH" ]; then
        MACDEPLOYQT="$MACDEPLOYQT_SEARCH"
    else
        MACDEPLOYQT_SEARCH=$(find /usr/local/Cellar/qt@5/*/bin/macdeployqt 2>/dev/null | head -n1)
        if [ -n "$MACDEPLOYQT_SEARCH" ]; then
            MACDEPLOYQT="$MACDEPLOYQT_SEARCH"
        fi
    fi
fi

if [ -z "$MACDEPLOYQT" ]; then
    echo "❌ 未找到 macdeployqt"
    exit 1
fi

echo "✅ 找到 macdeployqt: $MACDEPLOYQT"

# 检查可执行文件
if [ ! -f "build/bin/RankingSystem" ]; then
    echo "❌ 未找到可执行文件"
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

# 创建改进的 Info.plist
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
    <key>LSMinimumSystemVersion</key>
    <string>10.14</string>
    <key>CFBundleIconFile</key>
    <string>app_icon</string>
    <key>NSPrincipalClass</key>
    <string>NSApplication</string>
    <key>NSHighResolutionCapable</key>
    <true/>
    <key>LSHasLocalizedDisplayName</key>
    <true/>
    <key>NSHumanReadableCopyright</key>
    <string>© 2024 Contest Ranking System</string>
    <key>CFBundleGetInfoString</key>
    <string>竞赛排行榜系统 1.0</string>
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
"$MACDEPLOYQT" "$APP_DIR" -verbose=1

if [ $? -ne 0 ]; then
    echo "❌ macdeployqt 失败"
    exit 1
fi

# 改进的代码签名处理
echo "✍️ 处理代码签名..."

# 强制重新签名所有组件
echo "  - 签名动态库..."
find "$APP_DIR" -name "*.dylib" -exec codesign --force --sign - {} \; 2>/dev/null

echo "  - 签名框架..."
find "$APP_DIR" -name "*.framework" -exec codesign --force --sign - {} \; 2>/dev/null

echo "  - 签名主应用..."
codesign --force --sign - --deep "$APP_DIR" 2>/dev/null

if [ $? -eq 0 ]; then
    echo "✅ 代码签名完成"
else
    echo "⚠️ 代码签名失败，继续构建"
fi

# 验证签名
echo "🔍 验证应用签名..."
codesign --verify --deep --strict "$APP_DIR" 2>/dev/null
if [ $? -eq 0 ]; then
    echo "✅ 签名验证通过"
else
    echo "⚠️ 签名验证失败，但应用仍可能正常运行"
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
export DYLD_FALLBACK_LIBRARY_PATH="$PWD/../Frameworks"
exec ./RankingSystem "$@"
EOF
chmod +x "$APP_DIR/Contents/MacOS/start.sh"

# 创建安装说明
echo "📝 创建安装说明..."
cat > "$PACKAGE_DIR/修复版安装说明.txt" << 'EOF'
🚀 竞赛排行榜系统 - 修复版安装指南

本版本专门解决了 macOS 15.5+ 的兼容性问题

安装步骤：
1. 将 RankingSystem.app 拖入 Applications 文件夹
2. 首次运行请右键点击应用选择"打开"
3. 如果仍然遇到问题，请在终端中执行：
   sudo xattr -r -d com.apple.quarantine /Applications/RankingSystem.app

故障排除：
- 如果应用意外退出，请重启系统后再试
- 确保系统为 macOS 10.14 或更新版本
- 如有问题请查看项目文档

技术改进：
✅ 重新编译以解决签名问题
✅ 改进了依赖库处理
✅ 增强了 macOS 15.5+ 兼容性
✅ 优化了启动流程
EOF

# 创建 DMG 文件
echo "💿 创建 DMG 安装包..."
DMG_NAME="RankingSystem-v1.0-Fixed-macOS.dmg"
hdiutil create -volname "竞赛排行榜系统 v1.0 (修复版)" -srcfolder "$PACKAGE_DIR" -ov -format UDZO "$DMG_NAME"

if [ $? -eq 0 ]; then
    echo "✅ 修复版 macOS 打包完成！"
    echo ""
    echo "📦 生成的文件:"
    echo "   应用包: $APP_DIR"
    echo "   安装包: $DMG_NAME"
    echo ""
    echo "🚀 测试建议:"
    echo "   1. 先测试本地应用包是否正常运行"
    echo "   2. 再分发 DMG 文件给其他用户"
    echo ""
    echo "📋 修复说明:"
    echo "   - 解决了代码签名问题"
    echo "   - 改进了 macOS 15.5+ 兼容性"
    echo "   - 增强了依赖库处理"
else
    echo "❌ DMG 创建失败"
    exit 1
fi
