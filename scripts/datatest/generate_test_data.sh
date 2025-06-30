#!/bin/bash

# 测试数据生成脚本
# Test Data Generation Script

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
DATA_DIR="$SCRIPT_DIR/../data"

echo "=========================================="
echo "竞赛排行榜系统 - 测试数据生成器"
echo "RankingSystem - Test Data Generator"  
echo "=========================================="
echo

# 创建数据目录（如果不存在）
mkdir -p "$DATA_DIR"

# 生成随机时间戳
generate_timestamp() {
    local base_date="2024-07-01"
    local hour=$((8 + RANDOM % 8))  # 8-15点
    local minute=$((RANDOM % 60))
    local second=$((RANDOM % 60))
    printf "%sT%02d:%02d:%02d" "$base_date" "$hour" "$minute" "$second"
}

# 生成随机运行时间（毫秒）
generate_runtime() {
    local min_time=${1:-500}
    local max_time=${2:-5000}
    echo $((min_time + RANDOM % (max_time - min_time)))
}

# 生成随机内存使用（字节）
generate_memory() {
    local base_memory=${1:-64000}
    local multiplier=$((1 + RANDOM % 16))  # 1-16倍
    echo $((base_memory * multiplier))
}

# 生成单个团队数据
generate_team_data() {
    local team_id="$1"
    local team_name="$2"
    local num_problems=${3:-5}
    local success_rate=${4:-70}  # 成功率百分比
    
    local output_file="$DATA_DIR/${team_id}_results.json"
    local total_score=0
    local last_time=""
    local submissions=""
    
    echo "📝 生成团队数据: $team_name ($team_id)"
    
    # 生成提交记录
    for ((i=1; i<=num_problems; i++)); do
        local problem_id=$(printf "%c" $((64 + i)))  # A, B, C, ...
        local attempts=$((1 + RANDOM % 3))  # 1-3次尝试
        
        for ((j=1; j<=attempts; j++)); do
            local timestamp=$(generate_timestamp)
            local is_correct="false"
            local runtime=$(generate_runtime 1000 5000)
            local memory=$(generate_memory 128000)
            
            # 最后一次尝试有更高的成功率
            if [ $j -eq $attempts ] && [ $((RANDOM % 100)) -lt $success_rate ]; then
                is_correct="true"
                runtime=$(generate_runtime 500 3000)  # 成功的提交通常更快
                memory=$(generate_memory 64000)
                total_score=$((total_score + 100))
            fi
            
            last_time="$timestamp"
            
            if [ -n "$submissions" ]; then
                submissions="$submissions,"
            fi
            
            submissions="$submissions
        {
            \"problem_id\": \"$problem_id\",
            \"timestamp\": \"$timestamp\",
            \"is_correct\": $is_correct,
            \"run_time\": $runtime,
            \"memory_usage\": $memory
        }"
        done
    done
    
    # 生成JSON文件
    cat > "$output_file" << EOF
{
    "team_id": "$team_id",
    "team_name": "$team_name",
    "submissions": [$submissions
    ],
    "total_score": $total_score,
    "last_submit_time": "$last_time"
}
EOF
    
    echo "   ✅ 文件已生成: $output_file"
    echo "   📊 总分: $total_score"
}

# 生成哈希码
generate_hash() {
    local json_file="$1"
    local hash_file="${json_file}.sha256"
    
    cd "$DATA_DIR" || return 1
    local filename=$(basename "$json_file")
    local hash=$(shasum -a 256 "$filename" | cut -d' ' -f1)
    echo "$hash" > "$hash_file"
    echo "   🔒 哈希文件: $hash_file"
}

# 主函数
main() {
    echo "🚀 开始生成测试数据..."
    echo
    
    # 定义团队信息
    local teams=(
        "test_zeta:Zeta突击队:6:80"
        "test_eta:Eta智能组:7:90"
        "test_theta:Theta新星队:4:60"
        "test_iota:Iota挑战者:5:75"
        "test_kappa:Kappa精英团:8:95"
    )
    
    # 生成每个团队的数据
    for team_info in "${teams[@]}"; do
        IFS=':' read -r team_id team_name num_problems success_rate <<< "$team_info"
        generate_team_data "$team_id" "$team_name" "$num_problems" "$success_rate"
        generate_hash "$DATA_DIR/${team_id}_results.json"
        echo
    done
    
    # 更新综合校验和文件
    echo "🔄 更新综合校验和文件..."
    cd "$DATA_DIR" || exit 1
    shasum -a 256 *.json > data_checksums.sha256
    echo "   ✅ 已更新: data_checksums.sha256"
    echo
    
    # 显示统计信息
    local total_json=$(find "$DATA_DIR" -name "*.json" | wc -l)
    local total_hash=$(find "$DATA_DIR" -name "*.sha256" | wc -l)
    
    echo "📈 数据生成完成！"
    echo "Data generation completed!"
    echo "  - JSON 数据文件: $total_json"
    echo "  - SHA256 哈希文件: $total_hash"
    echo "  - 数据目录: $DATA_DIR"
    echo
    echo "💡 提示: 使用 scripts/verify_data_integrity.sh 验证数据完整性"
    echo "Tip: Use scripts/verify_data_integrity.sh to verify data integrity"
}

# 检查参数
if [ "$1" = "--help" ] || [ "$1" = "-h" ]; then
    echo "用法: $0 [选项]"
    echo "Usage: $0 [options]"
    echo
    echo "选项:"
    echo "Options:"
    echo "  -h, --help    显示此帮助信息"
    echo "                Show this help message"
    echo
    echo "此脚本会生成多个测试团队的JSON数据文件，"
    echo "每个文件都会包含对应的SHA256哈希码用于校验。"
    echo
    echo "This script generates JSON data files for multiple test teams,"
    echo "each with corresponding SHA256 hash for verification."
    exit 0
fi

# 执行主函数
main
