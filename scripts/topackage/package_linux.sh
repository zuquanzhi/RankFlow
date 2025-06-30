#!/bin/bash

echo "📦 竞赛排行榜系统 - Linux 打包脚本"
echo "===================================="

# 检查依赖
echo "🔍 检查打包依赖..."

# 检查可执行文件
if [ ! -f "build/bin/RankingSystem" ]; then
    echo "❌ 未找到可执行文件"
    echo "请先编译项目: ./build.sh"
    exit 1
fi

# 检查 linuxdeployqt (可选)
LINUXDEPLOY_URL="https://github.com/linuxdeploy/linuxdeploy/releases/download/continuous/linuxdeploy-x86_64.AppImage"
LINUXDEPLOY_QT_URL="https://github.com/linuxdeploy/linuxdeploy-plugin-qt/releases/download/continuous/linuxdeploy-plugin-qt-x86_64.AppImage"

echo "✅ 依赖检查完成"

# 创建打包目录
PACKAGE_DIR="package-linux"
APP_NAME="RankingSystem"

echo "📁 创建应用程序目录..."
rm -rf "$PACKAGE_DIR"
mkdir -p "$PACKAGE_DIR/$APP_NAME"

# 复制可执行文件
echo "📋 复制可执行文件..."
cp "build/bin/RankingSystem" "$PACKAGE_DIR/$APP_NAME/"

# 复制 Qt 库 (手动方式)
echo "🔗 复制 Qt 依赖库..."
mkdir -p "$PACKAGE_DIR/$APP_NAME/lib"

# 查找并复制 Qt 库
QT_LIBS=(
    "libQt5Core.so.5"
    "libQt5Gui.so.5" 
    "libQt5Widgets.so.5"
    "libQt5Charts.so.5"
    "libQt5Network.so.5"
    "libQt5DBus.so.5"
    "libQt5XcbQpa.so.5"
)

# 常见的 Qt 库路径
QT_LIB_PATHS=(
    "/usr/lib/x86_64-linux-gnu"
    "/usr/lib64"
    "/usr/local/lib"
    "/opt/Qt/5.15.2/gcc_64/lib"
)

for lib in "${QT_LIBS[@]}"; do
    found=false
    for path in "${QT_LIB_PATHS[@]}"; do
        if [ -f "$path/$lib" ]; then
            cp "$path/$lib" "$PACKAGE_DIR/$APP_NAME/lib/" 2>/dev/null || true
            found=true
            break
        fi
    done
    if [ "$found" = false ]; then
        echo "⚠️  未找到库: $lib"
    fi
done

# 复制 Qt 插件
echo "🔌 复制 Qt 插件..."
mkdir -p "$PACKAGE_DIR/$APP_NAME/plugins/platforms"
mkdir -p "$PACKAGE_DIR/$APP_NAME/plugins/imageformats"

# 查找 Qt 插件路径
QT_PLUGIN_PATHS=(
    "/usr/lib/x86_64-linux-gnu/qt5/plugins"
    "/usr/lib64/qt5/plugins"
    "/opt/Qt/5.15.2/gcc_64/plugins"
)

for path in "${QT_PLUGIN_PATHS[@]}"; do
    if [ -d "$path" ]; then
        cp -r "$path/platforms/"* "$PACKAGE_DIR/$APP_NAME/plugins/platforms/" 2>/dev/null || true
        cp -r "$path/imageformats/"* "$PACKAGE_DIR/$APP_NAME/plugins/imageformats/" 2>/dev/null || true
        break
    fi
done

# 复制数据文件和文档
echo "📂 复制数据文件和文档..."
cp -r data "$PACKAGE_DIR/$APP_NAME/"
cp -r docs "$PACKAGE_DIR/$APP_NAME/"
cp README.md "$PACKAGE_DIR/$APP_NAME/"
cp LICENSE "$PACKAGE_DIR/$APP_NAME/"

# 创建启动脚本
echo "📜 创建启动脚本..."
cat > "$PACKAGE_DIR/$APP_NAME/start.sh" << 'EOF'
#!/bin/bash
cd "$(dirname "$0")"

# 设置库路径
export LD_LIBRARY_PATH="./lib:$LD_LIBRARY_PATH"

# 设置 Qt 插件路径
export QT_PLUGIN_PATH="./plugins"

# 启动程序
./RankingSystem "$@"
EOF
chmod +x "$PACKAGE_DIR/$APP_NAME/start.sh"

# 创建桌面文件
echo "🖥️ 创建桌面快捷方式..."
cat > "$PACKAGE_DIR/$APP_NAME/RankingSystem.desktop" << EOF
[Desktop Entry]
Version=1.0
Type=Application
Name=竞赛排行榜系统
Name[en]=Ranking System
Comment=Programming Contest Ranking System
Comment[en]=Programming Contest Ranking System
Exec=./start.sh
Icon=./app_icon_256x256.png
Terminal=false
Categories=Development;Education;
StartupWMClass=RankingSystem
EOF

# 复制应用图标
echo "🎨 复制应用图标..."
if [ -f "resources/icons/app_icon_256x256.png" ]; then
    cp "resources/icons/app_icon_256x256.png" "$PACKAGE_DIR/$APP_NAME/"
    echo "✅ 图标复制成功"
else
    echo "⚠️ 未找到应用图标文件"
fi

# 创建安装脚本
echo "📦 创建安装脚本..."
cat > "$PACKAGE_DIR/install.sh" << 'EOF'
#!/bin/bash

echo "🚀 竞赛排行榜系统安装程序"
echo "=========================="
echo ""

INSTALL_DIR="/opt/RankingSystem"
DESKTOP_FILE="$HOME/.local/share/applications/RankingSystem.desktop"

# 检查权限
if [ "$EUID" -ne 0 ]; then
    echo "❌ 需要管理员权限安装到系统目录"
    echo "请使用: sudo ./install.sh"
    echo "或者手动运行: cd RankingSystem && ./start.sh"
    exit 1
fi

echo "正在安装到 $INSTALL_DIR..."

# 创建安装目录
mkdir -p "$INSTALL_DIR"

# 复制文件
cp -r RankingSystem/* "$INSTALL_DIR/"

# 创建系统级桌面文件
cat > "/usr/share/applications/RankingSystem.desktop" << EOL
[Desktop Entry]
Version=1.0
Type=Application
Name=竞赛排行榜系统
Name[en]=Ranking System
Comment=Programming Contest Ranking System
Comment[en]=Programming Contest Ranking System
Exec=$INSTALL_DIR/start.sh
Icon=applications-development
Terminal=false
Categories=Development;Education;
StartupWMClass=RankingSystem
EOL

# 创建命令行快捷方式
ln -sf "$INSTALL_DIR/start.sh" "/usr/local/bin/ranking-system"

echo "✅ 安装完成！"
echo ""
echo "🚀 启动方法:"
echo "   1. 从应用程序菜单启动"
echo "   2. 命令行运行: ranking-system"
echo "   3. 直接运行: $INSTALL_DIR/start.sh"
EOF
chmod +x "$PACKAGE_DIR/install.sh"

# 创建 AppImage (如果有 linuxdeploy)
echo "📱 尝试创建 AppImage..."
if command -v linuxdeploy-x86_64.AppImage &> /dev/null; then
    echo "找到 linuxdeploy，创建 AppImage..."
    
    # 创建 AppDir 结构
    APPDIR="$PACKAGE_DIR/RankingSystem.AppDir"
    mkdir -p "$APPDIR/usr/bin"
    mkdir -p "$APPDIR/usr/lib"
    
    cp "build/bin/RankingSystem" "$APPDIR/usr/bin/"
    cp -r data "$APPDIR/usr/bin/"
    
    # 创建 AppImage
    linuxdeploy-x86_64.AppImage --appdir "$APPDIR" --plugin qt --output appimage
    
    if [ -f "RankingSystem-x86_64.AppImage" ]; then
        mv "RankingSystem-x86_64.AppImage" "RankingSystem-v1.0-Linux.AppImage"
        chmod +x "RankingSystem-v1.0-Linux.AppImage"
        echo "✅ AppImage 创建成功: RankingSystem-v1.0-Linux.AppImage"
    fi
else
    echo "ℹ️  未找到 linuxdeploy，跳过 AppImage 创建"
    echo "   如需创建 AppImage，请下载 linuxdeploy："
    echo "   wget $LINUXDEPLOY_URL"
    echo "   chmod +x linuxdeploy-x86_64.AppImage"
fi

# 创建 TAR.GZ 压缩包
echo "📦 创建 TAR.GZ 压缩包..."
tar -czf "RankingSystem-v1.0-Linux.tar.gz" -C "$PACKAGE_DIR" .

echo "✅ Linux 打包完成！"
echo ""
echo "📦 生成的文件:"
echo "   应用目录: $PACKAGE_DIR/RankingSystem"
echo "   压缩包: RankingSystem-v1.0-Linux.tar.gz"
if [ -f "RankingSystem-v1.0-Linux.AppImage" ]; then
    echo "   AppImage: RankingSystem-v1.0-Linux.AppImage"
fi
echo ""
echo "🚀 使用方法:"
echo "   1. 解压: tar -xzf RankingSystem-v1.0-Linux.tar.gz"
echo "   2. 安装: sudo ./install.sh"
echo "   3. 或直接运行: cd RankingSystem && ./start.sh"
if [ -f "RankingSystem-v1.0-Linux.AppImage" ]; then
    echo "   4. 或运行 AppImage: ./RankingSystem-v1.0-Linux.AppImage"
fi
echo ""
echo "📋 分发说明:"
echo "   - 压缩包可分发给其他 Linux 用户"
echo "   - 支持主流 Linux 发行版"
echo "   - 包含所有依赖库"
