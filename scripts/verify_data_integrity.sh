#!/bin/bash

# 数据完整性校验脚本
# Data Integrity Verification Script

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DATA_DIR="$SCRIPT_DIR/../data"
CHECKSUM_FILE="$DATA_DIR/data_checksums.sha256"

echo "=========================================="
echo "竞赛排行榜系统 - 数据完整性校验"
echo "RankingSystem - Data Integrity Check"
echo "=========================================="
echo

# 检查校验和文件是否存在
if [ ! -f "$CHECKSUM_FILE" ]; then
    echo "❌ 错误: 校验和文件不存在: $CHECKSUM_FILE"
    echo "Error: Checksum file not found: $CHECKSUM_FILE"
    exit 1
fi

echo "📁 数据目录: $DATA_DIR"
echo "📋 校验和文件: $CHECKSUM_FILE"
echo

# 切换到数据目录
cd "$DATA_DIR" || {
    echo "❌ 错误: 无法进入数据目录: $DATA_DIR"
    echo "Error: Cannot enter data directory: $DATA_DIR"
    exit 1
}

echo "🔍 开始校验数据文件完整性..."
echo "Starting data integrity verification..."
echo

# 执行校验
if shasum -a 256 -c data_checksums.sha256; then
    echo
    echo "✅ 所有数据文件校验通过！"
    echo "All data files verified successfully!"
    echo
    
    # 显示文件统计
    total_files=$(wc -l < data_checksums.sha256)
    json_files=$(find . -name "*.json" | wc -l)
    hash_files=$(find . -name "*.sha256" | wc -l)
    
    echo "📊 数据文件统计："
    echo "File Statistics:"
    echo "  - JSON 数据文件: $json_files"
    echo "  - SHA256 哈希文件: $hash_files" 
    echo "  - 校验和记录总数: $total_files"
    echo
    
    exit 0
else
    echo
    echo "❌ 数据文件校验失败！"
    echo "Data file verification failed!"
    echo "请检查文件是否被修改或损坏。"
    echo "Please check if files have been modified or corrupted."
    echo
    exit 1
fi
