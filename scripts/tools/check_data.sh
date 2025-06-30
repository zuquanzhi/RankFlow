#!/bin/bash

echo "🔍 竞赛排行榜系统 - 数据导入诊断工具"
echo "================================================="

# 1. 检查数据目录
echo "📁 检查数据目录..."
if [ -d "data" ]; then
    echo "✅ data目录存在"
    echo "📊 当前数据文件列表:"
    ls -la data/*.json 2>/dev/null || echo "❌ 没有找到JSON文件"
    echo ""
else
    echo "❌ data目录不存在，正在创建..."
    mkdir -p data
fi

# 2. 验证JSON文件格式
echo "🔍 验证JSON文件格式..."
for file in data/*.json; do
    if [ -f "$file" ]; then
        echo "检查文件: $(basename "$file")"
        if python3 -m json.tool "$file" >/dev/null 2>&1; then
            echo "✅ $(basename "$file") - JSON格式正确"
        else
            echo "❌ $(basename "$file") - JSON格式错误"
            echo "   请检查文件内容"
        fi
    fi
done
echo ""

# 3. 检查程序运行状态
echo "🚀 检查程序运行状态..."
if pgrep -f RankingSystem >/dev/null; then
    echo "✅ RankingSystem正在运行"
    echo "进程信息:"
    ps aux | grep RankingSystem | grep -v grep
else
    echo "❌ RankingSystem未运行"
    echo "尝试启动程序..."
    if [ -f "build/bin/RankingSystem" ]; then
        echo "找到可执行文件，请手动运行: cd build && ./bin/RankingSystem"
    else
        echo "未找到可执行文件，请先编译: ./build.sh"
    fi
fi
echo ""

# 4. 测试文件监控
echo "🔄 测试文件监控功能..."
TEST_FILE="data/test_monitor.json"
echo "创建测试文件: $TEST_FILE"
cat > "$TEST_FILE" << 'EOF'
{
    "team_id": "test",
    "team_name": "监控测试队伍",
    "submissions": [
        {
            "problem_id": "TEST",
            "timestamp": "2025-06-30T10:00:00",
            "is_correct": true,
            "run_time": 1000,
            "memory_usage": 256000
        }
    ],
    "total_score": 100,
    "last_submit_time": "2025-06-30T10:00:00"
}
EOF

if [ -f "$TEST_FILE" ]; then
    echo "✅ 测试文件创建成功"
    echo "如果程序正在运行，应该会自动检测到这个新文件"
    echo "请在程序界面中检查是否出现了'监控测试队伍'"
else
    echo "❌ 测试文件创建失败"
fi
echo ""

# 5. 提供手动刷新建议
echo "💡 数据导入建议:"
echo "1. 确保程序正在运行"
echo "2. 在程序界面点击'手动刷新'按钮"
echo "3. 或者重新启动程序: cd build && ./bin/RankingSystem"
echo "4. 检查程序菜单 '查看' -> '查看审计日志' 了解详细信息"
echo ""

echo "🎯 快速测试步骤:"
echo "1. 运行程序: cd build && ./bin/RankingSystem"
echo "2. 观察界面是否显示6个队伍（包括新添加的team06和test）"
echo "3. 在程序中点击'手动刷新'按钮"
echo "4. 尝试修改任意JSON文件，保存后观察是否自动更新"
