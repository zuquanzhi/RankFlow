#!/bin/bash
cd "$(dirname "$0")"
if [ -f "bin/RankingSystem" ]; then
    ./bin/RankingSystem
else
    echo "错误: 找不到可执行文件，请先运行 build.sh 构建项目"
fi
