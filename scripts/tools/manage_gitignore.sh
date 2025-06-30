#!/bin/bash

# Git 忽略文件管理脚本

echo "🔧 Git 忽略文件管理工具"
echo "======================"

case "$1" in
    "check")
        echo "🔍 检查当前忽略状态..."
        echo ""
        echo "📁 被忽略的文件/目录："
        git status --ignored --porcelain | grep "^!!" | sed 's/^!! /  /' | head -10
        echo ""
        echo "📊 统计信息："
        ignored_count=$(git status --ignored --porcelain | grep "^!!" | wc -l)
        tracked_count=$(git ls-files | wc -l)
        echo "  忽略文件数量: $ignored_count"
        echo "  跟踪文件数量: $tracked_count"
        ;;
        
    "clean")
        echo "🧹 清理被忽略的文件..."
        echo ""
        echo "⚠️ 这将删除所有被 .gitignore 忽略的文件！"
        read -p "确定要继续吗？(y/N): " confirm
        if [[ $confirm =~ ^[Yy]$ ]]; then
            git clean -fdX
            echo "✅ 清理完成！"
        else
            echo "❌ 操作已取消"
        fi
        ;;
        
    "test")
        echo "🧪 测试文件是否被忽略..."
        if [ -n "$2" ]; then
            if git check-ignore "$2" >/dev/null 2>&1; then
                echo "✅ 文件 '$2' 会被忽略"
            else
                echo "❌ 文件 '$2' 不会被忽略"
            fi
        else
            echo "请提供要测试的文件路径"
            echo "用法: $0 test <文件路径>"
        fi
        ;;
        
    "simple")
        echo "📋 切换到简化版 .gitignore..."
        if [ -f ".gitignore.simple" ]; then
            cp .gitignore .gitignore.full
            cp .gitignore.simple .gitignore
            echo "✅ 已切换到简化版"
            echo "💾 完整版已备份为 .gitignore.full"
        else
            echo "❌ 未找到简化版文件"
        fi
        ;;
        
    "full")
        echo "📋 切换到完整版 .gitignore..."
        if [ -f ".gitignore.full" ]; then
            cp .gitignore.full .gitignore
            echo "✅ 已切换到完整版"
        else
            echo "❌ 未找到完整版备份"
        fi
        ;;
        
    "add")
        echo "➕ 添加忽略规则..."
        if [ -n "$2" ]; then
            echo "$2" >> .gitignore
            echo "✅ 已添加规则: $2"
        else
            echo "请提供要忽略的模式"
            echo "用法: $0 add <忽略模式>"
        fi
        ;;
        
    "list")
        echo "📋 当前忽略规则："
        echo ""
        cat .gitignore | grep -v "^#" | grep -v "^$" | nl
        ;;
        
    *)
        echo "用法: $0 <命令> [参数]"
        echo ""
        echo "可用命令："
        echo "  check     - 检查当前忽略状态"
        echo "  clean     - 清理被忽略的文件"
        echo "  test <文件> - 测试文件是否被忽略"
        echo "  simple    - 切换到简化版 .gitignore"
        echo "  full      - 切换到完整版 .gitignore"
        echo "  add <规则> - 添加新的忽略规则"
        echo "  list      - 列出当前所有忽略规则"
        echo ""
        echo "示例："
        echo "  $0 check"
        echo "  $0 test build/RankingSystem"
        echo "  $0 add '*.backup'"
        ;;
esac
