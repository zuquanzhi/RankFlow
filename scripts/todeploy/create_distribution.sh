#!/bin/bash

echo "📦 创建竞赛排行榜系统分发包"
echo "================================"

# 设置版本信息
VERSION="1.0"
BUILD_DATE=$(date +%Y%m%d)
PACKAGE_NAME="RankingSystem-v${VERSION}-${BUILD_DATE}"

# 创建分发目录
DIST_DIR="dist"
PACKAGE_DIR="$DIST_DIR/$PACKAGE_NAME"

echo "📁 创建分发包目录..."
rm -rf "$DIST_DIR"
mkdir -p "$PACKAGE_DIR"

# 复制核心文件
echo "📋 复制项目文件..."
cp -r src/ "$PACKAGE_DIR/"
cp -r ui/ "$PACKAGE_DIR/"
cp -r resources/ "$PACKAGE_DIR/"
cp -r data/ "$PACKAGE_DIR/"
cp -r docs/ "$PACKAGE_DIR/"
cp CMakeLists.txt "$PACKAGE_DIR/"

# 复制文档
echo "📖 复制文档文件..."
cp README.md "$PACKAGE_DIR/"
cp DEPLOYMENT_GUIDE.md "$PACKAGE_DIR/"
cp DATA_IMPORT_GUIDE.md "$PACKAGE_DIR/"
cp PACKAGING_GUIDE.md "$PACKAGE_DIR/"
cp PROJECT_SUMMARY.md "$PACKAGE_DIR/"
cp LICENSE "$PACKAGE_DIR/"

# 复制构建脚本
echo "🔨 复制构建脚本..."
cp build.sh "$PACKAGE_DIR/"
cp build.bat "$PACKAGE_DIR/"
cp check_data.sh "$PACKAGE_DIR/"

# 复制打包脚本
echo "📦 复制打包脚本..."
cp package_macos.sh "$PACKAGE_DIR/"
cp package_windows.bat "$PACKAGE_DIR/"
cp package_linux.sh "$PACKAGE_DIR/"
cp deploy.sh "$PACKAGE_DIR/"

# 创建分发包README
echo "📝 创建分发包说明..."
cat > "$PACKAGE_DIR/DISTRIBUTION_README.md" << EOF
# 🚀 竞赛排行榜系统 v${VERSION} - 分发包

## 📦 包含内容

此分发包包含完整的源代码、文档和部署工具，支持在各种平台上编译和部署。

### 📁 目录结构

\`\`\`
RankingSystem-v${VERSION}/
├── src/                    # 源代码
├── ui/                     # 界面文件
├── resources/              # 资源文件
├── data/                   # 示例数据
├── docs/                   # 技术文档
├── build.sh/.bat          # 编译脚本
├── package_*.sh/.bat      # 打包脚本
├── deploy.sh              # 快速部署脚本
├── check_data.sh          # 数据验证工具
└── *.md                   # 详细文档
\`\`\`

## 🚀 快速开始

### 1. 开发环境编译

**macOS/Linux:**
\`\`\`bash
./build.sh
\`\`\`

**Windows:**
\`\`\`cmd
build.bat
\`\`\`

### 2. 创建分发包

**macOS:**
\`\`\`bash
./package_macos.sh
\`\`\`

**Windows:**
\`\`\`cmd
package_windows.bat
\`\`\`

**Linux:**
\`\`\`bash
./package_linux.sh
\`\`\`

### 3. 快速部署到目标设备

\`\`\`bash
./deploy.sh
\`\`\`

## 📋 系统要求

### 开发环境
- Qt 5.12+ 
- CMake 3.16+
- C++17 编译器
- Git (可选)

### 运行环境
- 现代操作系统 (macOS 10.14+, Windows 10+, Linux)
- 4GB+ 内存
- 100MB+ 磁盘空间

## 📖 详细文档

- [部署指南](DEPLOYMENT_GUIDE.md) - 完整的安装和部署说明
- [数据导入指南](DATA_IMPORT_GUIDE.md) - 数据格式和导入方法
- [打包指南](PACKAGING_GUIDE.md) - 跨平台打包技术细节
- [项目总结](PROJECT_SUMMARY.md) - 功能概览和架构说明

## 🆘 技术支持

如在使用过程中遇到问题：

1. 查看相关文档和FAQ
2. 运行诊断脚本: \`./check_data.sh\`
3. 检查日志文件
4. 联系技术支持

## 📄 许可证

本软件遵循 MIT 许可证。详见 [LICENSE](LICENSE) 文件。

---

**构建信息:**
- 版本: v${VERSION}
- 构建日期: ${BUILD_DATE}
- 支持平台: macOS, Windows, Linux

🎉 感谢使用竞赛排行榜系统！
EOF

# 创建版本信息文件
echo "📊 创建版本信息..."
cat > "$PACKAGE_DIR/VERSION" << EOF
VERSION=${VERSION}
BUILD_DATE=${BUILD_DATE}
BUILD_COMMIT=$(git rev-parse --short HEAD 2>/dev/null || echo "unknown")
BUILD_PLATFORM=$(uname -s)-$(uname -m)
EOF

# 创建安装脚本
echo "⚙️ 创建安装脚本..."
cat > "$PACKAGE_DIR/install.sh" << 'EOF'
#!/bin/bash

echo "🚀 竞赛排行榜系统 - 安装脚本"
echo "=============================="

# 检查系统
OS="unknown"
if [[ "$OSTYPE" == "darwin"* ]]; then
    OS="macos"
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    OS="linux"
fi

echo "📱 操作系统: $OS"

# 检查依赖
echo "🔍 检查依赖..."
missing_deps=()

if ! command -v cmake &> /dev/null; then
    missing_deps+=("cmake")
fi

if [ "$OS" = "macos" ]; then
    if ! command -v brew &> /dev/null; then
        echo "❌ 需要安装 Homebrew: https://brew.sh"
        exit 1
    fi
    if ! brew list qt@5 &> /dev/null; then
        missing_deps+=("qt@5")
    fi
elif [ "$OS" = "linux" ]; then
    if ! pkg-config --exists Qt5Core; then
        missing_deps+=("qt5-default")
    fi
fi

if [ ${#missing_deps[@]} -ne 0 ]; then
    echo "❌ 缺少依赖: ${missing_deps[*]}"
    echo "请手动安装后重试"
    exit 1
fi

echo "✅ 依赖检查通过"

# 编译项目
echo "🔨 编译项目..."
if [ "$OS" = "macos" ] || [ "$OS" = "linux" ]; then
    ./build.sh
else
    echo "❌ 不支持的操作系统: $OS"
    exit 1
fi

if [ $? -eq 0 ]; then
    echo "✅ 编译成功"
    echo ""
    echo "🎯 下一步:"
    echo "  1. 运行 ./deploy.sh 配置用户环境"
    echo "  2. 参考 DEPLOYMENT_GUIDE.md 进行打包分发"
    echo ""
else
    echo "❌ 编译失败，请检查错误信息"
    exit 1
fi
EOF

chmod +x "$PACKAGE_DIR/install.sh"

# 清理不需要的文件
echo "🧹 清理临时文件..."
rm -rf "$PACKAGE_DIR"/build/
rm -rf "$PACKAGE_DIR"/logs/
rm -rf "$PACKAGE_DIR"/.vscode/
find "$PACKAGE_DIR" -name "*.log" -delete
find "$PACKAGE_DIR" -name ".DS_Store" -delete

# 创建压缩包
echo "📦 创建压缩包..."
cd "$DIST_DIR"
tar -czf "${PACKAGE_NAME}.tar.gz" "$PACKAGE_NAME"
zip -r "${PACKAGE_NAME}.zip" "$PACKAGE_NAME" > /dev/null

# 生成校验和
echo "🔐 生成校验和..."
if command -v sha256sum &> /dev/null; then
    sha256sum "${PACKAGE_NAME}".tar.gz > "${PACKAGE_NAME}.tar.gz.sha256"
    sha256sum "${PACKAGE_NAME}".zip > "${PACKAGE_NAME}.zip.sha256"
elif command -v shasum &> /dev/null; then
    shasum -a 256 "${PACKAGE_NAME}".tar.gz > "${PACKAGE_NAME}.tar.gz.sha256"
    shasum -a 256 "${PACKAGE_NAME}".zip > "${PACKAGE_NAME}.zip.sha256"
fi

cd ..

echo ""
echo "✅ 分发包创建完成！"
echo ""
echo "📦 生成的文件:"
echo "  📁 $PACKAGE_DIR/"
echo "  📦 $DIST_DIR/${PACKAGE_NAME}.tar.gz"
echo "  📦 $DIST_DIR/${PACKAGE_NAME}.zip"
echo ""
echo "📊 包大小:"
du -h "$DIST_DIR"/${PACKAGE_NAME}.*
echo ""
echo "🎯 分发说明:"
echo "  1. 将压缩包发送给目标用户"
echo "  2. 用户解压后运行 ./install.sh"
echo "  3. 参考 DISTRIBUTION_README.md 进行后续操作"
echo ""
echo "🎉 分发包已准备就绪！"
