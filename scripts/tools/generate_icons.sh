#!/bin/bash

# 图标生成脚本
# 从 SVG 生成各种格式的应用图标

echo "🎨 开始生成应用图标..."

# 检查依赖
if ! command -v rsvg-convert &> /dev/null && ! command -v inkscape &> /dev/null; then
    echo "❌ 需要安装 librsvg 或 inkscape 来转换 SVG"
    echo "安装方法："
    echo "  macOS: brew install librsvg"
    echo "  或者: brew install inkscape"
    exit 1
fi

# 设置路径
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "$SCRIPT_DIR/../.." && pwd)"
RESOURCES_DIR="$PROJECT_ROOT/resources"
ICONS_DIR="$RESOURCES_DIR/icons"
SVG_FILE="$ICONS_DIR/app_icon.svg"

# 检查 SVG 文件
if [ ! -f "$SVG_FILE" ]; then
    echo "❌ 未找到 SVG 文件: $SVG_FILE"
    exit 1
fi

# 创建输出目录
mkdir -p "$ICONS_DIR"

# 选择转换工具
CONVERTER=""
if command -v rsvg-convert &> /dev/null; then
    CONVERTER="rsvg-convert"
elif command -v inkscape &> /dev/null; then
    CONVERTER="inkscape"
fi

# 生成 PNG 函数
generate_png() {
    local size=$1
    local output="$ICONS_DIR/app_icon_${size}x${size}.png"
    
    echo "🔄 生成 ${size}x${size} PNG..."
    
    if [ "$CONVERTER" = "rsvg-convert" ]; then
        rsvg-convert -w $size -h $size "$SVG_FILE" -o "$output"
    else
        inkscape --export-width=$size --export-height=$size --export-filename="$output" "$SVG_FILE" 2>/dev/null
    fi
    
    if [ $? -eq 0 ]; then
        echo "✅ 生成成功: $output"
    else
        echo "❌ 生成失败: $output"
    fi
}

# 生成各种尺寸的 PNG
generate_png 16
generate_png 32
generate_png 48
generate_png 64
generate_png 128
generate_png 256
generate_png 512
generate_png 1024

# 生成 macOS icns 文件
echo "🍎 生成 macOS icns 文件..."
ICONSET_DIR="$ICONS_DIR/AppIcon.iconset"
rm -rf "$ICONSET_DIR"
mkdir -p "$ICONSET_DIR"

# 复制并重命名为 iconset 格式
cp "$ICONS_DIR/app_icon_16x16.png" "$ICONSET_DIR/icon_16x16.png"
cp "$ICONS_DIR/app_icon_32x32.png" "$ICONSET_DIR/icon_16x16@2x.png"
cp "$ICONS_DIR/app_icon_32x32.png" "$ICONSET_DIR/icon_32x32.png"
cp "$ICONS_DIR/app_icon_64x64.png" "$ICONSET_DIR/icon_32x32@2x.png"
cp "$ICONS_DIR/app_icon_128x128.png" "$ICONSET_DIR/icon_128x128.png"
cp "$ICONS_DIR/app_icon_256x256.png" "$ICONSET_DIR/icon_128x128@2x.png"
cp "$ICONS_DIR/app_icon_256x256.png" "$ICONSET_DIR/icon_256x256.png"
cp "$ICONS_DIR/app_icon_512x512.png" "$ICONSET_DIR/icon_256x256@2x.png"
cp "$ICONS_DIR/app_icon_512x512.png" "$ICONSET_DIR/icon_512x512.png"
cp "$ICONS_DIR/app_icon_1024x1024.png" "$ICONSET_DIR/icon_512x512@2x.png"

# 生成 icns 文件
if command -v iconutil &> /dev/null; then
    iconutil -c icns "$ICONSET_DIR" -o "$ICONS_DIR/app_icon.icns"
    if [ $? -eq 0 ]; then
        echo "✅ 生成 icns 成功: $ICONS_DIR/app_icon.icns"
    else
        echo "❌ 生成 icns 失败"
    fi
else
    echo "⚠️ iconutil 不可用，跳过 icns 生成"
fi

# 生成 Windows ico 文件
echo "🪟 生成 Windows ico 文件..."
if command -v convert &> /dev/null; then
    convert "$ICONS_DIR/app_icon_16x16.png" \
            "$ICONS_DIR/app_icon_32x32.png" \
            "$ICONS_DIR/app_icon_48x48.png" \
            "$ICONS_DIR/app_icon_64x64.png" \
            "$ICONS_DIR/app_icon_128x128.png" \
            "$ICONS_DIR/app_icon_256x256.png" \
            "$ICONS_DIR/app_icon.ico"
    
    if [ $? -eq 0 ]; then
        echo "✅ 生成 ico 成功: $ICONS_DIR/app_icon.ico"
    else
        echo "❌ 生成 ico 失败"
    fi
else
    echo "⚠️ ImageMagick convert 不可用，跳过 ico 生成"
    echo "   安装方法: brew install imagemagick"
fi

# 清理临时文件
rm -rf "$ICONSET_DIR"

echo ""
echo "🎉 图标生成完成！"
echo "📁 生成的文件："
ls -la "$ICONS_DIR"/ | grep -E "\.(png|icns|ico)$"

echo ""
echo "🔧 使用说明："
echo "  macOS 应用: 使用 app_icon.icns"
echo "  Windows 应用: 使用 app_icon.ico"
echo "  Linux 应用: 使用 app_icon_*.png"
echo "  Web 应用: 使用 app_icon_*.png"
