#!/bin/bash

# Quick Build Script - 快速构建脚本
# 项目根目录快捷构建入口

echo "🔨 开始构建 Qt5/C++17 编程竞赛排行榜系统..."

# 检查是否在项目根目录
if [ ! -f "CMakeLists.txt" ]; then
    echo "❌ 错误：请在项目根目录运行此脚本"
    exit 1
fi

# 调用实际的构建脚本
exec ./scripts/build/build.sh
