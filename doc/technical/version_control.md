# 版本控制指南

本文档详细说明了 Qt5/C++17 编程竞赛排行榜系统的版本控制配置和最佳实践。

## 📁 .gitignore 配置

项目包含了完整的 `.gitignore` 配置，涵盖了以下类别：

### 🔨 构建相关文件
```gitignore
build/
build-*/
*.o
*.obj
*.a
*.lib
*.so
*.dll
*.exe
```

### 🔧 Qt 开发文件
```gitignore
*.pro.user*
ui_*.h
qrc_*.cpp
moc_*.cpp
moc_*.h
Makefile*
```

### 📦 CMake 文件
```gitignore
CMakeCache.txt
CMakeFiles/
cmake_install.cmake
```

### 💻 IDE 和编辑器
```gitignore
.vscode/
.vs/
.idea/
*.user
*.swp
.DS_Store
```

### 📦 打包和分发
```gitignore
package/
dist/
*.dmg
*.zip
*.tar.gz
```

### 📊 运行时文件
```gitignore
logs/
*.log
*.tmp
*.cache
```

## 🛠️ .gitignore 管理工具

项目提供了便捷的 `.gitignore` 管理脚本：

### 基本用法
```bash
# 检查当前忽略状态
./scripts/tools/manage_gitignore.sh check

# 测试特定文件是否被忽略
./scripts/tools/manage_gitignore.sh test "build/RankingSystem"

# 列出所有忽略规则
./scripts/tools/manage_gitignore.sh list
```

### 版本切换
```bash
# 切换到简化版 .gitignore（适合小项目）
./scripts/tools/manage_gitignore.sh simple

# 切换回完整版 .gitignore
./scripts/tools/manage_gitignore.sh full
```

### 规则管理
```bash
# 添加新的忽略规则
./scripts/tools/manage_gitignore.sh add "*.backup"

# 清理所有被忽略的文件（谨慎使用）
./scripts/tools/manage_gitignore.sh clean
```

## 📋 版本控制最佳实践

### 提交指南

#### 提交信息格式
使用语义化的提交信息：

```
<类型>(<范围>): <描述>

[可选的正文]

[可选的脚注]
```

#### 提交类型
- `feat`: 新功能
- `fix`: 错误修复
- `docs`: 文档更新
- `style`: 代码格式化
- `refactor`: 代码重构
- `test`: 测试相关
- `chore`: 构建/工具链更新

#### 示例
```bash
git commit -m "feat(ui): 添加实时排行榜刷新功能"
git commit -m "fix(data): 修复 JSON 数据解析异常"
git commit -m "docs(readme): 更新安装指南"
```

### 分支策略

#### 主要分支
- `main`: 稳定发布版本
- `develop`: 开发分支
- `feature/*`: 功能分支
- `bugfix/*`: 错误修复分支
- `release/*`: 发布准备分支

#### 工作流程
```bash
# 创建功能分支
git checkout -b feature/new-chart-widget develop

# 开发完成后合并
git checkout develop
git merge --no-ff feature/new-chart-widget
git branch -d feature/new-chart-widget
```

### 文件管理

#### 应该提交的文件
- ✅ 源代码文件 (`src/*.cpp`, `src/*.h`)
- ✅ 用户界面文件 (`ui/*.ui`)
- ✅ 资源文件 (`resources/*.qrc`, `resources/icons/*.svg`)
- ✅ 构建配置 (`CMakeLists.txt`)
- ✅ 文档文件 (`doc/*.md`, `README.md`)
- ✅ 脚本文件 (`scripts/*.sh`, `scripts/*.bat`)

#### 不应提交的文件
- ❌ 构建产物 (`build/`, `*.exe`, `*.o`)
- ❌ IDE 配置 (`.vscode/`, `.idea/`)
- ❌ 打包文件 (`package/`, `*.dmg`)
- ❌ 日志文件 (`logs/`, `*.log`)
- ❌ 系统文件 (`.DS_Store`, `Thumbs.db`)

### 敏感信息处理

#### 配置文件
```gitignore
# 本地配置文件
config/local.conf
.env
.env.local
```

#### 测试数据
```gitignore
# 真实比赛数据
data/contest_real.json
data/teams_private.json
```

## 🔧 Git 钩子

项目可以配置 Git 钩子来自动化常见任务：

### pre-commit 钩子
```bash
#!/bin/bash
# 在提交前运行代码格式检查
echo "正在运行代码格式检查..."
if ! command -v clang-format &> /dev/null; then
    echo "警告: 未找到 clang-format"
    exit 0
fi

# 检查 C++ 文件格式
find src/ -name "*.cpp" -o -name "*.h" | xargs clang-format --dry-run --Werror
```

### post-receive 钩子
```bash
#!/bin/bash
# 在接收推送后自动部署
echo "正在自动部署..."
cd /path/to/deployment
git pull origin main
./scripts/deploy/deploy.sh
```

## 📊 Git 操作示例

### 日常开发
```bash
# 检查状态
git status

# 查看忽略的文件
git status --ignored

# 添加文件
git add src/newfeature.cpp

# 提交更改
git commit -m "feat(ranking): 添加实时更新功能"

# 推送到远程
git push origin feature/realtime-updates
```

### 版本发布
```bash
# 创建发布分支
git checkout -b release/v1.1.0 develop

# 更新版本号
echo "1.1.0" > VERSION

# 提交版本更新
git commit -m "chore(release): 准备 v1.1.0 发布"

# 合并到主分支
git checkout main
git merge --no-ff release/v1.1.0

# 创建标签
git tag -a v1.1.0 -m "发布版本 1.1.0"

# 推送标签
git push origin v1.1.0
```

### 问题排查
```bash
# 查看文件历史
git log --follow src/mainwindow.cpp

# 查看特定提交的更改
git show <commit-hash>

# 比较分支差异
git diff main..develop

# 查找引入问题的提交
git bisect start
git bisect bad HEAD
git bisect good v1.0.0
```

## 🚀 持续集成

建议配置 CI/CD 流程：

### GitHub Actions 示例
```yaml
name: Build and Test

on: [push, pull_request]

jobs:
  build:
    runs-on: ubuntu-latest
    steps:
    - uses: actions/checkout@v2
    - name: Install Qt
      run: sudo apt-get install qt5-default
    - name: Build
      run: ./scripts/build/build.sh
    - name: Test
      run: ./scripts/tools/test_icons.sh
```

---

遵循这些版本控制最佳实践将帮助您更好地管理项目代码，确保代码质量和团队协作效率。
