#!/bin/bash

echo "🔧 竞赛排行榜系统 - 安装修复脚本"
echo "====================================="

APP_PATH="/Applications/RankingSystem.app"

# 检查应用是否存在
if [ ! -d "$APP_PATH" ]; then
    echo "❌ 未找到应用：$APP_PATH"
    echo "请确保已将应用安装到Applications文件夹"
    exit 1
fi

echo "✅ 找到应用：$APP_PATH"

# 移除隔离属性
echo "🔓 移除安全隔离属性..."
sudo xattr -r -d com.apple.quarantine "$APP_PATH" 2>/dev/null

if [ $? -eq 0 ]; then
    echo "✅ 隔离属性已移除"
else
    echo "⚠️ 移除隔离属性失败或不需要"
fi

# 检查可执行权限
echo "🔍 检查执行权限..."
chmod +x "$APP_PATH/Contents/MacOS/RankingSystem"

# 尝试启动应用
echo "🚀 尝试启动应用..."
open "$APP_PATH"

if [ $? -eq 0 ]; then
    echo "✅ 应用启动成功！"
    echo ""
    echo "🎉 修复完成！应用现在应该可以正常运行了。"
else
    echo "❌ 应用启动失败"
    echo ""
    echo "🆘 如果仍有问题，请尝试："
    echo "   1. 右键点击应用 → 选择'打开'"
    echo "   2. 查看系统偏好设置 → 安全性与隐私"
    echo "   3. 联系技术支持"
fi

echo ""
echo "📖 更多帮助请查看项目文档或DMG中的安装说明文件"
