#!/bin/bash

echo "🚀 竞赛排行榜系统 - 快速部署脚本"
echo "====================================="

# 检测操作系统
OS="unknown"
if [[ "$OSTYPE" == "darwin"* ]]; then
    OS="macos"
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    OS="linux"
elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "cygwin" ]]; then
    OS="windows"
fi

echo "📱 检测到操作系统: $OS"

# 设置数据目录
if [ "$OS" = "macos" ] || [ "$OS" = "linux" ]; then
    DATA_DIR="$HOME/Documents/RankingSystem"
else
    DATA_DIR="$USERPROFILE/Documents/RankingSystem"
fi

echo "📁 数据目录: $DATA_DIR"

# 创建目录结构
echo "📋 创建目录结构..."
mkdir -p "$DATA_DIR"/{data,logs,config,backup}

# 创建示例配置文件
echo "⚙️ 创建配置文件..."
cat > "$DATA_DIR/config/config.json" << 'EOF'
{
    "refreshInterval": 5000,
    "dataDirectory": "./data",
    "logLevel": "INFO",
    "ui": {
        "theme": "dark",
        "language": "zh-CN",
        "chartAnimation": true,
        "danmakuEnabled": true,
        "fullscreen": false
    },
    "security": {
        "validateData": true,
        "backupEnabled": true,
        "backupInterval": 300000
    },
    "display": {
        "autoRefresh": true,
        "showProblemDetails": true,
        "showTeamLogos": false,
        "maxDisplayTeams": 50
    }
}
EOF

# 创建示例数据文件
echo "📊 创建示例数据..."
cat > "$DATA_DIR/data/team01_results.json" << 'EOF'
{
    "teamName": "示例队伍1",
    "members": ["选手A", "选手B", "选手C"],
    "totalScore": 300,
    "solvedCount": 3,
    "penalty": 30,
    "lastSubmission": "2024-01-15T14:30:00",
    "problems": [
        {
            "id": "A",
            "title": "简单题",
            "status": "accepted",
            "score": 100,
            "attempts": 1,
            "solveTime": 900,
            "submissions": [
                {
                    "time": "2024-01-15T10:15:00",
                    "status": "accepted",
                    "score": 100
                }
            ]
        },
        {
            "id": "B",
            "title": "中等题",
            "status": "accepted",
            "score": 100,
            "attempts": 2,
            "solveTime": 1800,
            "submissions": [
                {
                    "time": "2024-01-15T11:00:00",
                    "status": "wrong_answer",
                    "score": 0
                },
                {
                    "time": "2024-01-15T11:30:00",
                    "status": "accepted",
                    "score": 100
                }
            ]
        },
        {
            "id": "C",
            "title": "困难题",
            "status": "accepted",
            "score": 100,
            "attempts": 3,
            "solveTime": 3600,
            "submissions": [
                {
                    "time": "2024-01-15T12:00:00",
                    "status": "time_limit_exceeded",
                    "score": 0
                },
                {
                    "time": "2024-01-15T13:00:00",
                    "status": "wrong_answer",
                    "score": 0
                },
                {
                    "time": "2024-01-15T14:30:00",
                    "status": "accepted",
                    "score": 100
                }
            ]
        }
    ]
}
EOF

# 创建数据验证脚本
echo "🔍 创建数据验证脚本..."
cat > "$DATA_DIR/validate_data.sh" << 'EOF'
#!/bin/bash
echo "🔍 验证竞赛数据..."

DATA_DIR="$(dirname "$0")/data"
VALID_COUNT=0
ERROR_COUNT=0

if [ ! -d "$DATA_DIR" ]; then
    echo "❌ 数据目录不存在: $DATA_DIR"
    exit 1
fi

echo "📁 检查数据目录: $DATA_DIR"

for file in "$DATA_DIR"/*.json; do
    if [ -f "$file" ]; then
        echo -n "检查 $(basename "$file")... "
        if python3 -m json.tool "$file" > /dev/null 2>&1; then
            echo "✅"
            ((VALID_COUNT++))
        else
            echo "❌ JSON格式错误"
            ((ERROR_COUNT++))
        fi
    fi
done

echo ""
echo "📊 验证结果:"
echo "  ✅ 有效文件: $VALID_COUNT"
echo "  ❌ 错误文件: $ERROR_COUNT"

if [ $ERROR_COUNT -eq 0 ]; then
    echo "🎉 所有数据文件验证通过！"
    exit 0
else
    echo "⚠️ 存在格式错误的文件，请检查修正"
    exit 1
fi
EOF

chmod +x "$DATA_DIR/validate_data.sh"

# 创建启动脚本
echo "🚀 创建启动脚本..."
if [ "$OS" = "macos" ]; then
    cat > "$DATA_DIR/start_ranking.sh" << 'EOF'
#!/bin/bash
cd "$(dirname "$0")"
if [ -f "/Applications/RankingSystem.app/Contents/MacOS/RankingSystem" ]; then
    echo "🚀 启动竞赛排行榜系统..."
    /Applications/RankingSystem.app/Contents/MacOS/RankingSystem
else
    echo "❌ 未找到排行榜程序，请先安装 RankingSystem.app"
    echo "💡 将 .app 文件拖入 /Applications/ 文件夹"
fi
EOF
elif [ "$OS" = "linux" ]; then
    cat > "$DATA_DIR/start_ranking.sh" << 'EOF'
#!/bin/bash
cd "$(dirname "$0")"
if command -v RankingSystem &> /dev/null; then
    echo "🚀 启动竞赛排行榜系统..."
    RankingSystem
elif [ -f "./RankingSystem" ]; then
    echo "🚀 启动竞赛排行榜系统..."
    ./RankingSystem
elif [ -f "./RankingSystem-*.AppImage" ]; then
    echo "🚀 启动竞赛排行榜系统 (AppImage)..."
    ./RankingSystem-*.AppImage
else
    echo "❌ 未找到排行榜程序"
    echo "💡 请将程序文件放在此目录或安装到系统"
fi
EOF
fi

chmod +x "$DATA_DIR/start_ranking.sh"

# 创建README
echo "📝 创建使用说明..."
cat > "$DATA_DIR/README.md" << 'EOF'
# 竞赛排行榜系统 - 用户数据目录

## 📁 目录结构

```
RankingSystem/
├── data/           # 队伍数据文件 (*.json)
├── config/         # 配置文件
├── logs/           # 程序日志
├── backup/         # 数据备份
├── validate_data.sh # 数据验证脚本
├── start_ranking.sh # 启动脚本
└── README.md       # 本说明文件
```

## 🚀 快速开始

1. **数据导入**: 将队伍JSON文件放入 `data/` 目录
2. **验证数据**: 运行 `./validate_data.sh`
3. **启动程序**: 运行 `./start_ranking.sh`

## 📊 数据格式

队伍数据文件命名: `teamXX_results.json`

基本格式:
```json
{
    "teamName": "队伍名称",
    "members": ["成员1", "成员2"],
    "totalScore": 分数,
    "solvedCount": 解题数,
    "penalty": 罚时,
    "lastSubmission": "最后提交时间",
    "problems": [题目数据...]
}
```

## ⚙️ 配置选项

编辑 `config/config.json` 可修改:
- 刷新频率
- 界面主题
- 显示选项
- 日志级别

## 🔧 故障排除

1. 数据无法加载 → 运行验证脚本检查格式
2. 程序无法启动 → 检查程序是否正确安装
3. 界面异常 → 查看 `logs/` 目录中的日志文件

更多帮助请参考项目文档。
EOF

echo ""
echo "✅ 部署完成！"
echo ""
echo "📍 数据目录已创建在: $DATA_DIR"
echo ""
echo "🎯 下一步操作:"
echo "  1. 安装竞赛排行榜程序到系统"
echo "  2. 将队伍数据文件放入 data/ 目录"
echo "  3. 运行验证脚本: cd '$DATA_DIR' && ./validate_data.sh"
echo "  4. 启动程序: cd '$DATA_DIR' && ./start_ranking.sh"
echo ""
echo "📖 详细说明请查看: $DATA_DIR/README.md"
echo ""
echo "🎉 祝您使用愉快！"
