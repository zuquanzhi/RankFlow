@echo off
echo 📦 竞赛排行榜系统 - Windows 打包脚本
echo ==========================================

REM 检查依赖
echo 🔍 检查打包依赖...

REM 检查 Qt 安装
where windeployqt >nul 2>nul
if %errorlevel% neq 0 (
    echo ❌ 未找到 windeployqt
    echo 请确保 Qt5 正确安装并添加到 PATH 环境变量
    pause
    exit /b 1
)

REM 检查可执行文件
if not exist "build\bin\RankingSystem.exe" (
    echo ❌ 未找到可执行文件
    echo 请先编译项目: build.bat
    pause
    exit /b 1
)

echo ✅ 依赖检查完成

REM 创建打包目录
set PACKAGE_DIR=package-windows
set APP_NAME=RankingSystem

echo 📁 创建应用程序目录...
if exist "%PACKAGE_DIR%" rmdir /s /q "%PACKAGE_DIR%"
mkdir "%PACKAGE_DIR%\%APP_NAME%"

REM 复制可执行文件
echo 📋 复制可执行文件...
copy "build\bin\RankingSystem.exe" "%PACKAGE_DIR%\%APP_NAME%\"

REM 使用 windeployqt 处理依赖
echo 🔗 处理 Qt 依赖...
cd "%PACKAGE_DIR%\%APP_NAME%"
windeployqt.exe RankingSystem.exe --qmldir ..\..\src

if %errorlevel% neq 0 (
    echo ❌ windeployqt 失败
    pause
    exit /b 1
)

cd ..\..

REM 复制数据文件和文档
echo 📂 复制数据文件和文档...
xcopy /E /I data "%PACKAGE_DIR%\%APP_NAME%\data"
xcopy /E /I docs "%PACKAGE_DIR%\%APP_NAME%\docs"
copy README.md "%PACKAGE_DIR%\%APP_NAME%\"
copy LICENSE "%PACKAGE_DIR%\%APP_NAME%\"

REM 复制应用图标
echo 🎨 复制应用图标...
if exist "resources\icons\app_icon_256x256.png" (
    copy "resources\icons\app_icon_256x256.png" "%PACKAGE_DIR%\%APP_NAME%\"
    echo ✅ 图标复制成功
) else (
    echo ⚠️ 未找到应用图标文件
)

REM 创建启动脚本
echo 📜 创建启动脚本...
echo @echo off > "%PACKAGE_DIR%\%APP_NAME%\start.bat"
echo cd /d "%%~dp0" >> "%PACKAGE_DIR%\%APP_NAME%\start.bat"
echo RankingSystem.exe >> "%PACKAGE_DIR%\%APP_NAME%\start.bat"

REM 创建安装脚本
echo 📦 创建安装脚本...
echo @echo off > "%PACKAGE_DIR%\install.bat"
echo echo 🚀 竞赛排行榜系统安装程序 >> "%PACKAGE_DIR%\install.bat"
echo echo ========================== >> "%PACKAGE_DIR%\install.bat"
echo echo. >> "%PACKAGE_DIR%\install.bat"
echo echo 正在安装到 C:\Program Files\RankingSystem... >> "%PACKAGE_DIR%\install.bat"
echo xcopy /E /I /Y "%APP_NAME%" "C:\Program Files\RankingSystem\" >> "%PACKAGE_DIR%\install.bat"
echo echo. >> "%PACKAGE_DIR%\install.bat"
echo echo ✅ 安装完成！ >> "%PACKAGE_DIR%\install.bat"
echo echo 可以从开始菜单或桌面快捷方式启动程序 >> "%PACKAGE_DIR%\install.bat"
echo pause >> "%PACKAGE_DIR%\install.bat"

REM 创建卸载脚本
echo 🗑️ 创建卸载脚本...
echo @echo off > "%PACKAGE_DIR%\%APP_NAME%\uninstall.bat"
echo echo 🗑️ 竞赛排行榜系统卸载程序 >> "%PACKAGE_DIR%\%APP_NAME%\uninstall.bat"
echo echo ========================== >> "%PACKAGE_DIR%\%APP_NAME%\uninstall.bat"
echo echo. >> "%PACKAGE_DIR%\%APP_NAME%\uninstall.bat"
echo set /p confirm="确定要卸载竞赛排行榜系统吗？(Y/N): " >> "%PACKAGE_DIR%\%APP_NAME%\uninstall.bat"
echo if /i "%%confirm%%"=="Y" ( >> "%PACKAGE_DIR%\%APP_NAME%\uninstall.bat"
echo     echo 正在卸载... >> "%PACKAGE_DIR%\%APP_NAME%\uninstall.bat"
echo     rd /s /q "C:\Program Files\RankingSystem" >> "%PACKAGE_DIR%\%APP_NAME%\uninstall.bat"
echo     echo ✅ 卸载完成！ >> "%PACKAGE_DIR%\%APP_NAME%\uninstall.bat"
echo ) >> "%PACKAGE_DIR%\%APP_NAME%\uninstall.bat"
echo pause >> "%PACKAGE_DIR%\%APP_NAME%\uninstall.bat"

REM 创建 ZIP 压缩包
echo 📦 创建 ZIP 压缩包...
powershell -command "Compress-Archive -Path '%PACKAGE_DIR%\*' -DestinationPath 'RankingSystem-v1.0-Windows.zip' -Force"

if %errorlevel% eq 0 (
    echo ✅ Windows 打包完成！
    echo.
    echo 📦 生成的文件:
    echo    应用目录: %PACKAGE_DIR%\%APP_NAME%
    echo    压缩包: RankingSystem-v1.0-Windows.zip
    echo.
    echo 🚀 使用方法:
    echo    1. 解压 RankingSystem-v1.0-Windows.zip
    echo    2. 运行 install.bat 安装到系统
    echo    3. 或直接运行 %APP_NAME%\RankingSystem.exe
    echo.
    echo 📋 分发说明:
    echo    - ZIP 文件可直接分发给其他 Windows 用户
    echo    - 支持 Windows 10+ 系统
    echo    - 包含所有依赖，无需额外安装
) else (
    echo ❌ ZIP 创建失败
)

pause
