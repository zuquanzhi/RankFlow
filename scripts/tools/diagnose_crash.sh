#!/bin/bash

echo "🔍 RankingSystem 意外退出诊断脚本"
echo "===================================="

# 基本信息
echo "📊 系统信息:"
echo "操作系统: $(sw_vers -productName) $(sw_vers -productVersion)"
echo "架构: $(uname -m)"
echo ""

# 检查应用文件
APP_PATH="/Applications/RankingSystem.app"
LOCAL_APP_PATH="package/RankingSystem.app"

echo "📁 应用检查:"
if [ -d "$APP_PATH" ]; then
    echo "✅ 已安装: $APP_PATH"
    EXEC_PATH="$APP_PATH/Contents/MacOS/RankingSystem"
elif [ -d "$LOCAL_APP_PATH" ]; then
    echo "✅ 本地包: $LOCAL_APP_PATH"
    EXEC_PATH="$LOCAL_APP_PATH/Contents/MacOS/RankingSystem"
else
    echo "❌ 未找到应用"
    exit 1
fi

# 检查可执行文件
echo ""
echo "⚙️ 可执行文件检查:"
if [ -f "$EXEC_PATH" ]; then
    echo "✅ 可执行文件存在: $EXEC_PATH"
    echo "文件大小: $(ls -lh "$EXEC_PATH" | awk '{print $5}')"
    echo "权限: $(ls -l "$EXEC_PATH" | awk '{print $1}')"
else
    echo "❌ 可执行文件不存在"
    exit 1
fi

# 检查代码签名
echo ""
echo "🔐 代码签名检查:"
codesign -dv "$EXEC_PATH" 2>&1

# 检查依赖库
echo ""
echo "📚 依赖库检查:"
echo "Qt Frameworks:"
for framework in QtCore QtGui QtWidgets QtCharts QtNetwork; do
    FRAMEWORK_PATH="$(dirname "$EXEC_PATH")/../Frameworks/$framework.framework/Versions/5/$framework"
    if [ -f "$FRAMEWORK_PATH" ]; then
        echo "✅ $framework"
    else
        echo "❌ $framework 缺失"
    fi
done

# 检查数据目录
echo ""
echo "📂 数据目录检查:"
DATA_DIR="$HOME/Documents/RankingSystem"
if [ -d "$DATA_DIR" ]; then
    echo "✅ 数据目录存在: $DATA_DIR"
    echo "数据文件数量: $(ls "$DATA_DIR/data"/*.json 2>/dev/null | wc -l | tr -d ' ')"
else
    echo "⚠️ 数据目录不存在，创建中..."
    mkdir -p "$DATA_DIR/data"
    mkdir -p "$DATA_DIR/logs"
    echo "✅ 数据目录已创建"
fi

# 检查权限和隔离属性
echo ""
echo "🔒 安全属性检查:"
QUARANTINE=$(xattr -l "$EXEC_PATH" 2>/dev/null | grep quarantine)
if [ -n "$QUARANTINE" ]; then
    echo "⚠️ 检测到隔离属性: $QUARANTINE"
    echo "建议执行: sudo xattr -r -d com.apple.quarantine '$(dirname "$EXEC_PATH" | sed 's|/Contents/MacOS||')'"
else
    echo "✅ 无隔离属性"
fi

# 尝试启动并捕获错误
echo ""
echo "🚀 启动测试:"
echo "尝试启动应用..."

# 设置环境变量
export QT_LOGGING_RULES="*.debug=false"
export QT_QPA_PLATFORM_PLUGIN_PATH="$(dirname "$EXEC_PATH")/../PlugIns/platforms"

# 创建启动脚本
TEMP_SCRIPT="/tmp/test_ranking_startup.sh"
cat > "$TEMP_SCRIPT" << EOF
#!/bin/bash
cd "\$(dirname "$EXEC_PATH")"
exec "$EXEC_PATH" "\$@"
EOF
chmod +x "$TEMP_SCRIPT"

echo "运行命令: $TEMP_SCRIPT"
echo "输出:"
echo "----------------------------------------"

# 使用timeout限制运行时间，避免挂起
timeout 10s "$TEMP_SCRIPT" 2>&1 &
PID=$!

# 等待进程结束或超时
wait $PID 2>/dev/null
EXIT_CODE=$?

echo "----------------------------------------"
echo "退出代码: $EXIT_CODE"

if [ $EXIT_CODE -eq 124 ]; then
    echo "✅ 应用启动成功（10秒后自动关闭）"
elif [ $EXIT_CODE -eq 0 ]; then
    echo "✅ 应用正常退出"
else
    echo "❌ 应用异常退出 (代码: $EXIT_CODE)"
fi

# 检查崩溃日志
echo ""
echo "📋 崩溃日志检查:"
CRASH_LOGS=$(ls ~/Library/Logs/DiagnosticReports/RankingSystem* 2>/dev/null | head -3)
if [ -n "$CRASH_LOGS" ]; then
    echo "⚠️ 发现崩溃日志:"
    for log in $CRASH_LOGS; do
        echo "  - $(basename "$log")"
    done
    echo "最新崩溃日志内容:"
    echo "----------------------------------------"
    head -20 $(echo "$CRASH_LOGS" | head -1) 2>/dev/null
    echo "----------------------------------------"
else
    echo "✅ 无崩溃日志"
fi

# 检查系统日志
echo ""
echo "📰 系统日志检查:"
echo "查找最近的RankingSystem相关日志..."
log show --predicate 'processImagePath contains "RankingSystem"' --info --last 1m 2>/dev/null | tail -10

echo ""
echo "🎯 诊断建议:"

if [ $EXIT_CODE -ne 0 ] && [ $EXIT_CODE -ne 124 ]; then
    echo "❌ 应用无法正常启动，建议:"
    echo "  1. 检查系统是否满足要求 (macOS 10.14+)"
    echo "  2. 重新安装应用"
    echo "  3. 清除隔离属性: sudo xattr -r -d com.apple.quarantine '$(dirname "$EXEC_PATH" | sed 's|/Contents/MacOS||')'"
    echo "  4. 检查是否有足够的内存和磁盘空间"
    echo "  5. 重启系统后再试"
else
    echo "✅ 应用能够启动，可能是："
    echo "  1. 数据文件问题 - 检查JSON格式"
    echo "  2. 界面显示问题 - 尝试不同的显示器设置"
    echo "  3. 内存不足 - 关闭其他应用"
fi

echo ""
echo "📞 如需技术支持，请提供此诊断报告"

# 清理临时文件
rm -f "$TEMP_SCRIPT"
