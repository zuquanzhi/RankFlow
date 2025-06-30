# RankingSystem 脚本使用指南

## 脚本概览

本项目提供了完整的自动化脚本集合，涵盖编译、打包、部署、维护等各个环节，帮助开发者和运维人员高效管理项目。

## 脚本分类

### 🔨 构建编译脚本

| 脚本名称 | 平台 | 功能 | 状态 |
|----------|------|------|------|
| `build.sh` | macOS/Linux | 编译项目 | ✅ 可用 |
| `build.bat` | Windows | 编译项目 | ✅ 可用 |
| `clean.sh` | 通用 | 清理构建文件 | ✅ 可用 |

### 📦 打包分发脚本

| 脚本名称 | 平台 | 输出格式 | 功能 | 状态 |
|----------|------|----------|------|------|
| `package_macos.sh` | macOS | .app + .dmg | 标准macOS打包 | ✅ 可用 |
| `package_macos_fixed.sh` | macOS | .app + .dmg | 修复版macOS打包 | ✅ 推荐 |
| `package_windows.bat` | Windows | .exe + .zip | Windows打包 | ✅ 可用 |
| `package_linux.sh` | Linux | .AppImage + .tar.gz | Linux打包 | ✅ 可用 |
| `create_distribution.sh` | 通用 | 源码包 | 源码分发 | ✅ 可用 |

### 🛠️ 维护工具脚本

| 脚本名称 | 功能 | 用途 | 状态 |
|----------|------|------|------|
| `fix_crash_issue.sh` | 修复macOS崩溃问题 | 故障排除 | ✅ 可用 |
| `diagnose_crash.sh` | 诊断应用崩溃 | 问题分析 | ✅ 可用 |
| `fix_macos_app.sh` | 修复macOS应用权限 | 快速修复 | ✅ 可用 |
| `check_data.sh` | 检查数据完整性 | 数据验证 | ✅ 可用 |

## 详细使用说明

### 构建编译脚本

#### build.sh / build.bat

**功能**：编译整个项目，生成可执行文件

**用法**：
```bash
# macOS/Linux
chmod +x build.sh
./build.sh

# Windows
build.bat
```

**输出**：
- `build/bin/RankingSystem`：可执行文件
- `build/`：构建临时文件

**选项**：
```bash
# 清理后重新构建
./build.sh clean

# 调试模式构建
./build.sh debug

# 发布模式构建
./build.sh release
```

#### clean.sh

**功能**：清理所有构建文件和临时文件

**用法**：
```bash
./clean.sh
```

**清理内容**：
- `build/` 目录
- 临时文件
- 编译缓存
- 日志文件

### 打包分发脚本

#### package_macos_fixed.sh（推荐）

**功能**：为macOS创建经过修复的应用包，解决崩溃问题

**用法**：
```bash
chmod +x package_macos_fixed.sh
./package_macos_fixed.sh
```

**输出**：
- `package/RankingSystem.app`：应用程序包
- `RankingSystem-v1.0-Fixed-macOS.dmg`：分发镜像

**特性**：
- 统一深度签名
- 完善的Info.plist
- 兼容macOS 15.5+
- 自动依赖处理

#### package_windows.bat

**功能**：为Windows创建分发包

**用法**：
```cmd
package_windows.bat
```

**输出**：
- `package-windows/`：应用目录
- `RankingSystem-v1.0-Windows.zip`：压缩包

**包含内容**：
- 主程序和依赖库
- 资源文件和数据
- 文档和许可证
- 启动脚本

#### package_linux.sh

**功能**：为Linux创建分发包

**用法**：
```bash
chmod +x package_linux.sh
./package_linux.sh
```

**输出**：
- `package-linux/RankingSystem/`：应用目录
- `RankingSystem-v1.0-Linux.tar.gz`：压缩包
- `RankingSystem-x86_64.AppImage`：便携版（如果配置了linuxdeployqt）

#### create_distribution.sh

**功能**：创建源码分发包

**用法**：
```bash
./create_distribution.sh
```

**输出**：
- `RankingSystem-v1.0-Source.tar.gz`：源码包
- `dist/`：分发目录

### 维护工具脚本

#### fix_crash_issue.sh

**功能**：自动修复macOS下的应用崩溃问题

**用法**：
```bash
./fix_crash_issue.sh
```

**修复内容**：
- 重新编译应用
- 修复代码签名
- 完善应用配置
- 重新打包分发

**输出**：修复后的DMG文件

#### diagnose_crash.sh

**功能**：诊断应用崩溃原因

**用法**：
```bash
./diagnose_crash.sh [应用路径]
```

**检查内容**：
- 代码签名状态
- 依赖库完整性
- 系统兼容性
- 权限设置
- 崩溃日志分析

**输出**：详细诊断报告

#### fix_macos_app.sh

**功能**：快速修复macOS应用的常见问题

**用法**：
```bash
./fix_macos_app.sh [应用路径]
```

**修复操作**：
- 移除隔离属性
- 修复文件权限
- 尝试启动应用
- 验证修复结果

#### check_data.sh

**功能**：检查数据文件的完整性和格式

**用法**：
```bash
./check_data.sh [数据目录]
```

**检查内容**：
- JSON格式验证
- 文件完整性
- 数据一致性
- 权限检查

## 脚本配置

### 环境变量

```bash
# 设置Qt路径（如果非标准安装）
export QT_DIR="/usr/local/opt/qt@5"

# 设置构建类型
export BUILD_TYPE="Release"  # 或 "Debug"

# 设置版本号
export APP_VERSION="1.0.0"

# 设置应用名称
export APP_NAME="RankingSystem"
```

### 配置文件

创建 `scripts/config.sh`：

```bash
#!/bin/bash
# 脚本全局配置

# 应用信息
APP_NAME="RankingSystem"
APP_VERSION="1.0.0"
APP_IDENTIFIER="com.company.rankingsystem"

# 路径配置
SOURCE_DIR="$(pwd)"
BUILD_DIR="$SOURCE_DIR/build"
PACKAGE_DIR="$SOURCE_DIR/package"
DIST_DIR="$SOURCE_DIR/dist"

# Qt路径
case "$(uname -s)" in
    Darwin*)
        QT_DIR="/usr/local/opt/qt@5"
        ;;
    Linux*)
        QT_DIR="/usr/lib/x86_64-linux-gnu/qt5"
        ;;
esac

# 构建选项
BUILD_TYPE="Release"
PARALLEL_JOBS=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# 打包选项
ENABLE_SIGNING=false
SIGNING_IDENTITY=""
```

### 自定义脚本

创建自定义构建脚本 `scripts/custom_build.sh`：

```bash
#!/bin/bash
source "$(dirname "$0")/config.sh"

echo "🔨 Custom build process for $APP_NAME v$APP_VERSION"

# 预构建检查
echo "📋 Pre-build checks..."
if [ ! -f "CMakeLists.txt" ]; then
    echo "❌ CMakeLists.txt not found!"
    exit 1
fi

# 清理旧构建
echo "🧹 Cleaning old build..."
rm -rf "$BUILD_DIR"
mkdir -p "$BUILD_DIR"

# 配置构建
echo "⚙️ Configuring build..."
cd "$BUILD_DIR"
cmake .. -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
         -DCMAKE_PREFIX_PATH="$QT_DIR"

# 编译
echo "🔨 Building..."
make -j"$PARALLEL_JOBS"

# 后构建验证
echo "✅ Post-build verification..."
if [ -f "bin/$APP_NAME" ]; then
    echo "✅ Build successful!"
    echo "📍 Executable: $BUILD_DIR/bin/$APP_NAME"
else
    echo "❌ Build failed!"
    exit 1
fi
```

## 故障排除

### 常见问题

1. **权限错误**
   ```bash
   chmod +x script_name.sh
   ```

2. **Qt路径问题**
   ```bash
   export QT_DIR="/path/to/qt"
   export PATH="$QT_DIR/bin:$PATH"
   ```

3. **依赖缺失**
   ```bash
   # macOS
   brew install qt@5 cmake
   
   # Ubuntu
   sudo apt install qt5-default libqt5charts5-dev cmake
   ```

4. **构建失败**
   ```bash
   # 清理后重新构建
   ./clean.sh
   ./build.sh
   ```

### 调试模式

启用脚本调试：

```bash
# 显示详细执行过程
bash -x script_name.sh

# 在脚本中添加调试信息
set -x  # 启用调试
set +x  # 关闭调试
```

### 日志记录

脚本执行日志：

```bash
# 记录脚本输出
./script_name.sh 2>&1 | tee script.log

# 只记录错误
./script_name.sh 2> error.log

# 记录到系统日志
logger "Script execution: $0"
```

## 最佳实践

### 脚本编写规范

1. **错误处理**
   ```bash
   set -e  # 遇错退出
   set -u  # 未定义变量报错
   set -o pipefail  # 管道错误处理
   ```

2. **参数验证**
   ```bash
   if [ $# -lt 1 ]; then
       echo "Usage: $0 <argument>"
       exit 1
   fi
   ```

3. **路径处理**
   ```bash
   SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
   cd "$SCRIPT_DIR"
   ```

### 版本管理

```bash
# 版本号管理
get_version() {
    if git describe --tags >/dev/null 2>&1; then
        git describe --tags --abbrev=0
    else
        echo "1.0.0"
    fi
}

APP_VERSION=$(get_version)
```

### 跨平台兼容

```bash
# 平台检测
get_platform() {
    case "$(uname -s)" in
        Darwin*) echo "macos" ;;
        Linux*)  echo "linux" ;;
        CYGWIN*|MINGW*|MSYS*) echo "windows" ;;
        *) echo "unknown" ;;
    esac
}

PLATFORM=$(get_platform)
```

---

**脚本指南版本：** v1.0  
**适用项目版本：** RankingSystem v1.0+  
**最后更新：** 2025年6月30日  
**维护团队：** 自动化组
