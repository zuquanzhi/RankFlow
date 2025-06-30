#!/bin/bash

# 竞赛排行榜系统构建脚本

echo "开始构建竞赛排行榜系统..."

# 检查必要的工具
command -v cmake >/dev/null 2>&1 || { echo "错误: 需要安装CMake" >&2; exit 1; }

# 检查Qt安装
if [[ "$OSTYPE" == "darwin"* ]]; then
    # macOS
    if ! command -v qmake >/dev/null 2>&1; then
        echo "正在查找Qt安装..."
        QT_FOUND=false
        
        # 检查常见的Qt安装路径
        QT_PATHS=(
            "/usr/local/opt/qt5"
            "/opt/homebrew/opt/qt5"
            "/usr/local/opt/qt@5"
            "/opt/homebrew/opt/qt@5"
            "/Applications/Qt/5.15.2/clang_64"
            "/Applications/Qt/*/clang_64"
            "$HOME/Qt/5.*/clang_64"
        )
        
        for path in "${QT_PATHS[@]}"; do
            if [ -d "$path" ] && [ -f "$path/bin/qmake" ]; then
                echo "找到Qt安装: $path"
                export CMAKE_PREFIX_PATH="$path:$CMAKE_PREFIX_PATH"
                export PATH="$path/bin:$PATH"
                QT_FOUND=true
                break
            fi
        done
        
        if [ "$QT_FOUND" = false ]; then
            echo "错误: 未找到Qt5安装"
            echo "请安装Qt5，可以通过以下方式之一:"
            echo "  1. Homebrew: brew install qt5"
            echo "  2. 官方安装包: https://www.qt.io/download"
            echo "  3. 手动设置Qt路径: export CMAKE_PREFIX_PATH=/path/to/qt5"
            exit 1
        fi
    fi
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    # Linux
    command -v qmake >/dev/null 2>&1 || { echo "错误: 需要安装Qt5开发包: sudo apt-get install qtbase5-dev qtchooser qt5-qmake qtbase5-dev-tools
sudo apt-get install libqt5charts5-dev" >&2; exit 1; }
fi

# 创建构建目录
if [ -d "build" ]; then
    echo "清理现有构建目录..."
    rm -rf build
fi

mkdir build
cd build

# 配置项目
echo "配置CMake项目..."
cmake .. -DCMAKE_BUILD_TYPE=Release

# 检查配置是否成功
if [ $? -ne 0 ]; then
    echo "错误: CMake配置失败"
    exit 1
fi

# 编译项目
echo "编译项目..."
if [[ "$OSTYPE" == "darwin"* ]] || [[ "$OSTYPE" == "linux-gnu"* ]]; then
    make -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
else
    cmake --build . --config Release
fi

# 检查编译是否成功
if [ $? -ne 0 ]; then
    echo "错误: 编译失败"
    exit 1
fi

echo "构建成功！"
echo "可执行文件位置: build/bin/RankingSystem"

# 创建运行脚本
cat > run.sh << 'EOF'
#!/bin/bash
cd "$(dirname "$0")"
if [ -f "bin/RankingSystem" ]; then
    ./bin/RankingSystem
else
    echo "错误: 找不到可执行文件，请先运行 build.sh 构建项目"
fi
EOF

chmod +x run.sh

echo "创建了运行脚本: build/run.sh"
echo ""
echo "使用方法:"
echo "  直接运行: cd build && ./bin/RankingSystem"
echo "  或使用脚本: cd build && ./run.sh"
