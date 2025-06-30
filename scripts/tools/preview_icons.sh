#!/bin/bash

# 图标预览脚本 - 显示生成的图标信息

echo "🎨 应用图标预览"
echo "=============="
echo ""

ICONS_DIR="resources/icons"

if [ ! -d "$ICONS_DIR" ]; then
    echo "❌ 图标目录不存在: $ICONS_DIR"
    echo "请先运行: ./scripts/tools/generate_icons.sh"
    exit 1
fi

echo "📁 图标文件列表："
echo "├── SVG 源文件:"
ls -la "$ICONS_DIR"/*.svg 2>/dev/null | awk '{print "│   " $9 " (" $5 " bytes)"}'

echo "├── PNG 文件:"
ls -la "$ICONS_DIR"/*.png 2>/dev/null | awk '{print "│   " $9 " (" $5 " bytes)"}'

echo "├── macOS 图标:"
ls -la "$ICONS_DIR"/*.icns 2>/dev/null | awk '{print "│   " $9 " (" $5 " bytes)"}'

echo "└── Windows 图标:"
ls -la "$ICONS_DIR"/*.ico 2>/dev/null | awk '{print "    " $9 " (" $5 " bytes)"}'

echo ""
echo "🔍 图标尺寸信息："

for size in 16 32 48 64 128 256 512 1024; do
    file="$ICONS_DIR/app_icon_${size}x${size}.png"
    if [ -f "$file" ]; then
        filesize=$(ls -lh "$file" | awk '{print $5}')
        echo "  ${size}×${size}px: $filesize"
    fi
done

echo ""
echo "📱 平台支持："
echo "  ✅ macOS: app_icon.icns (支持 Retina 显示)"
echo "  ✅ Linux: app_icon_256x256.png (桌面文件)"
echo "  ✅ Windows: app_icon_*.png (应用资源)"
echo "  ✅ Qt 应用: 内嵌资源 :/icons/app_icon_*.png"

# 检查图标是否在资源文件中
echo ""
echo "🔧 集成状态："
if grep -q "app_icon" resources/resources.qrc 2>/dev/null; then
    echo "  ✅ 已集成到 Qt 资源文件"
else
    echo "  ❌ 未集成到 Qt 资源文件"
fi

if grep -q "app_icon" src/*.cpp 2>/dev/null; then
    echo "  ✅ 源代码中已引用图标"
else
    echo "  ❌ 源代码中未引用图标"
fi

# 检查系统工具
echo ""
echo "🛠️ 系统工具："
if command -v open &> /dev/null; then
    echo "  💡 macOS 预览图标: open $ICONS_DIR/app_icon_256x256.png"
fi

if command -v xdg-open &> /dev/null; then
    echo "  💡 Linux 预览图标: xdg-open $ICONS_DIR/app_icon_256x256.png"
fi

echo ""
echo "📋 快速操作："
echo "  🔄 重新生成图标: ./scripts/tools/generate_icons.sh"
echo "  🧪 测试图标集成: ./scripts/tools/test_icons.sh"
echo "  📦 打包应用: ./scripts/package/package_macos.sh"
