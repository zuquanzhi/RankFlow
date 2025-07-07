#!/bin/bash
# 竞赛排行榜系统网络功能演示脚本

echo "🚀 竞赛排行榜系统网络功能演示"
echo "=================================="

# 检查依赖
echo "📋 检查环境..."
if ! command -v python3 &> /dev/null; then
    echo "❌ Python3 未安装"
    exit 1
fi

echo "✅ Python3 已安装"

# 启动测试服务器
echo "🌐 启动测试服务器..."
cd "$(dirname "$0")"
python3 test_server.py &
SERVER_PID=$!
echo "✅ 测试服务器已启动 (PID: $SERVER_PID)"

# 等待服务器启动
echo "⏳ 等待服务器启动..."
sleep 3

# 测试网络连接
echo "🔍 测试网络连接..."
if curl -s http://localhost:8080/api/ping > /dev/null; then
    echo "✅ 网络连接正常"
else
    echo "❌ 网络连接失败"
    kill $SERVER_PID 2>/dev/null
    exit 1
fi

# 启动应用程序
echo "🎮 启动排行榜应用程序..."
cd ../build
export QT_LOGGING_RULES="*.debug=false"
./bin/RankingSystem &
APP_PID=$!
echo "✅ 应用程序已启动 (PID: $APP_PID)"

echo ""
echo "🎯 演示说明："
echo "1. 测试服务器运行在: http://localhost:8080"
echo "2. 应用程序已启动，请进行以下操作："
echo "   - 点击工具栏中的'网络配置'按钮"
echo "   - 配置服务器地址: http://localhost:8080"
echo "   - 设置API端点: /api/teams"
echo "   - 点击'测试连接'验证"
echo "   - 将数据源切换为'网络实时'"
echo "   - 观察排行榜数据实时更新"
echo ""
echo "3. 可用的API端点："
echo "   - http://localhost:8080/api/teams     # 队伍数据"
echo "   - http://localhost:8080/api/contest   # 竞赛信息"
echo "   - http://localhost:8080/api/problems  # 题目列表"
echo "   - http://localhost:8080/api/ping      # 连接测试"
echo ""
echo "📊 实时数据："
echo "   - 服务器每10秒自动更新队伍分数"
echo "   - 应用程序可设置自动刷新间隔"
echo "   - 支持自动重连和错误处理"
echo ""

# 等待用户输入
echo "按 Enter 键查看网络功能测试结果..."
read

# 运行网络功能测试
echo "🧪 运行网络功能测试..."
python3 test_network_functionality.py

echo ""
echo "按 Enter 键停止演示..."
read

# 清理进程
echo "🧹 清理进程..."
kill $APP_PID 2>/dev/null && echo "✅ 应用程序已停止"
kill $SERVER_PID 2>/dev/null && echo "✅ 测试服务器已停止"

echo "✨ 演示完成！"
