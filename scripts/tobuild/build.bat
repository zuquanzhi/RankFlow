@echo off
echo 开始构建竞赛排行榜系统...

REM 检查CMake是否安装
where cmake >nul 2>nul
if %errorlevel% neq 0 (
    echo 错误: 需要安装CMake
    pause
    exit /b 1
)

REM 检查Qt是否安装
where qmake >nul 2>nul
if %errorlevel% neq 0 (
    echo 错误: 需要安装Qt5并将其添加到PATH环境变量中
    pause
    exit /b 1
)

REM 创建构建目录
if exist build (
    echo 清理现有构建目录...
    rmdir /s /q build
)

mkdir build
cd build

REM 配置项目
echo 配置CMake项目...
cmake .. -DCMAKE_BUILD_TYPE=Release

if %errorlevel% neq 0 (
    echo 错误: CMake配置失败
    pause
    exit /b 1
)

REM 编译项目
echo 编译项目...
cmake --build . --config Release

if %errorlevel% neq 0 (
    echo 错误: 编译失败
    pause
    exit /b 1
)

echo 构建成功！
echo 可执行文件位置: build\bin\RankingSystem.exe

REM 创建运行脚本
echo @echo off > run.bat
echo cd /d "%%~dp0" >> run.bat
echo if exist "bin\RankingSystem.exe" ( >> run.bat
echo     bin\RankingSystem.exe >> run.bat
echo ) else ( >> run.bat
echo     echo 错误: 找不到可执行文件，请先运行 build.bat 构建项目 >> run.bat
echo     pause >> run.bat
echo ) >> run.bat

echo 创建了运行脚本: build\run.bat
echo.
echo 使用方法:
echo   直接运行: cd build ^&^& bin\RankingSystem.exe
echo   或使用脚本: cd build ^&^& run.bat

pause
