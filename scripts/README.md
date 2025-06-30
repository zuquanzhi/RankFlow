# 脚本目录说明

本目录包含了 Qt5/C++17 编程竞赛排行榜系统的所有构建、打包、部署和维护脚本。

## 目录结构

```
scripts/
├── README.md           # 本说明文件
├── build/              # 构建脚本
├── package/            # 打包脚本
├── deploy/             # 部署脚本
└── tools/              # 工具脚本
```

## 各目录详细说明

### 📦 build/ - 构建脚本
包含项目编译构建相关的脚本：
- `build.sh` - Linux/macOS 构建脚本
- `build.bat` - Windows 构建脚本

### 📦 package/ - 打包脚本
包含各平台应用打包相关的脚本：
- `package_linux.sh` - Linux 平台打包脚本
- `package_macos.sh` - macOS 平台打包脚本
- `package_macos_fixed.sh` - macOS 平台修复版打包脚本
- `package_windows.bat` - Windows 平台打包脚本

### 🚀 deploy/ - 部署脚本
包含项目部署和分发相关的脚本：
- `deploy.sh` - 通用部署脚本
- `create_distribution.sh` - 分发包创建脚本

### 🔧 tools/ - 工具脚本
包含项目维护和故障排除相关的脚本：
- `check_data.sh` - 数据完整性检查脚本
- `diagnose_crash.sh` - 崩溃诊断脚本
- `fix_crash_issue.sh` - 崩溃问题修复脚本
- `fix_macos_app.sh` - macOS 应用修复脚本
- `generate_icons.sh` - 应用图标生成脚本

## 使用方法

### 快速构建
```bash
# Linux/macOS
./scripts/build/build.sh

# Windows
scripts\build\build.bat
```

### 打包应用
```bash
# Linux
./scripts/package/package_linux.sh

# macOS
./scripts/package/package_macos.sh

# Windows
scripts\package\package_windows.bat
```

### 部署应用
```bash
./scripts/deploy/deploy.sh
```

### 故障排除
```bash
# 检查数据完整性
./scripts/tools/check_data.sh

# 诊断崩溃问题
./scripts/tools/diagnose_crash.sh

# 修复 macOS 应用问题
./scripts/tools/fix_macos_app.sh

# 重新生成应用图标
./scripts/tools/generate_icons.sh
```

## 注意事项

1. **权限设置**：确保脚本具有执行权限
   ```bash
   chmod +x scripts/**/*.sh
   ```

2. **环境要求**：
   - Linux/macOS：需要 bash shell 环境
   - Windows：需要 cmd 或 PowerShell 环境
   - 所有平台：需要安装 Qt5 开发环境

3. **路径问题**：从项目根目录执行脚本，避免路径相关问题

4. **依赖检查**：运行脚本前确保系统已安装必要的依赖

## 维护说明

- 所有脚本都应包含适当的错误处理和日志输出
- 新增脚本时请更新本文档
- 定期检查脚本的兼容性和有效性
- 遵循统一的编码规范和命名约定

---

最后更新：2025年6月30日
