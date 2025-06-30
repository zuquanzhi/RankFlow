#!/bin/bash

# 图标集成测试脚本
echo "🎨 测试应用图标集成..."

# 检查资源文件
echo "🔍 检查资源文件..."
if [ -f "resources/resources.qrc" ]; then
    echo "✅ 找到 resources.qrc"
    grep -q "app_icon" resources/resources.qrc
    if [ $? -eq 0 ]; then
        echo "✅ 资源文件中包含应用图标"
    else
        echo "❌ 资源文件中未找到应用图标"
    fi
else
    echo "❌ 未找到 resources.qrc 文件"
fi

# 检查源代码中的图标引用
echo "🔍 检查源代码中的图标引用..."
grep -r "app_icon" src/ --include="*.cpp" --include="*.h" > /dev/null
if [ $? -eq 0 ]; then
    echo "✅ 源代码中包含图标引用"
    echo "📋 图标引用位置："
    grep -r "app_icon" src/ --include="*.cpp" --include="*.h" | head -5
else
    echo "❌ 源代码中未找到图标引用"
fi

# 检查打包脚本中的图标处理
echo "🔍 检查打包脚本中的图标处理..."
for script in scripts/package/*.sh scripts/package/*.bat; do
    if [ -f "$script" ]; then
        echo "📄 检查 $(basename "$script")..."
        grep -q "icon\|图标" "$script"
        if [ $? -eq 0 ]; then
            echo "✅ $(basename "$script") 包含图标处理"
        else
            echo "⚠️ $(basename "$script") 未包含图标处理"
        fi
    fi
done

echo ""
echo "🎉 图标集成检查完成！"
