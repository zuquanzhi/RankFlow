# RankingSystem 开发指南

## 开发环境搭建

### 系统要求

- **操作系统**：Windows 10+, macOS 10.14+, Ubuntu 18.04+
- **编译器**：
  - GCC 7+ (Linux)
  - Clang 8+ (macOS) 
  - MSVC 2019+ (Windows)
- **构建工具**：CMake 3.16+
- **版本控制**：Git

### 依赖安装

#### Ubuntu/Debian

```bash
# 更新包列表
sudo apt update

# 安装Qt5开发包
sudo apt install qt5-default qt5-qmake qtbase5-dev qtchooser

# 安装Qt Charts
sudo apt install libqt5charts5-dev

# 安装CMake
sudo apt install cmake build-essential

# 安装Git
sudo apt install git
```

#### macOS

```bash
# 安装Homebrew（如果未安装）
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# 安装Qt5
brew install qt@5

# 安装CMake
brew install cmake

# 设置Qt环境变量
echo 'export PATH="/usr/local/opt/qt@5/bin:$PATH"' >> ~/.zshrc
source ~/.zshrc
```

#### Windows

1. **安装Qt**：
   - 下载Qt在线安装器：https://www.qt.io/download
   - 选择Qt 5.15.2 LTS版本
   - 确保勾选Qt Charts组件

2. **安装Visual Studio**：
   - 下载Visual Studio 2019 Community
   - 勾选"使用C++的桌面开发"工作负载

3. **安装CMake**：
   - 下载并安装CMake：https://cmake.org/download/
   - 确保添加到系统PATH

## 项目结构

```
RankingSystem/
├── CMakeLists.txt          # 主构建文件
├── README.md               # 项目说明
├── LICENSE                 # 许可证
├── src/                    # 源代码
│   ├── main.cpp           # 程序入口
│   ├── mainwindow.cpp     # 主窗口
│   ├── mainwindow.h
│   ├── teamdata.cpp       # 队伍数据
│   ├── teamdata.h
│   ├── datamanager.cpp    # 数据管理
│   ├── datamanager.h
│   ├── chartwidget.cpp    # 图表组件
│   ├── chartwidget.h
│   └── ...
├── ui/                     # UI文件
│   ├── mainwindow.ui
│   └── ...
├── resources/              # 资源文件
│   ├── icons/
│   ├── images/
│   └── resources.qrc
├── data/                   # 示例数据
│   ├── team01.json
│   └── ...
├── tests/                  # 测试代码
│   ├── CMakeLists.txt
│   ├── test_teamdata.cpp
│   └── ...
├── scripts/                # 构建脚本
│   ├── build.sh
│   ├── package_macos.sh
│   └── ...
└── doc/                    # 文档
    ├── user/
    ├── technical/
    └── ...
```

## 构建系统

### CMake 配置

主 `CMakeLists.txt` 文件：

```cmake
cmake_minimum_required(VERSION 3.16)
project(RankingSystem VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置Qt相关
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 查找Qt包
find_package(Qt5 REQUIRED COMPONENTS
    Core
    Widgets
    Charts
    Network
)

# 源文件
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/teamdata.cpp
    src/datamanager.cpp
    src/chartwidget.cpp
    src/problemwidget.cpp
    src/danmakuwidget.cpp
)

# 头文件
set(HEADERS
    src/mainwindow.h
    src/teamdata.h
    src/datamanager.h
    src/chartwidget.h
    src/problemwidget.h
    src/danmakuwidget.h
)

# UI文件
set(UI_FILES
    ui/mainwindow.ui
)

# 资源文件
set(RESOURCES
    resources/resources.qrc
)

# 创建可执行文件
add_executable(RankingSystem
    ${SOURCES}
    ${HEADERS}
    ${UI_FILES}
    ${RESOURCES}
)

# 链接Qt库
target_link_libraries(RankingSystem
    Qt5::Core
    Qt5::Widgets
    Qt5::Charts
    Qt5::Network
)

# 设置输出目录
set_target_properties(RankingSystem PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# 安装规则
install(TARGETS RankingSystem
    RUNTIME DESTINATION bin
)

# 测试
enable_testing()
add_subdirectory(tests)
```

### 构建步骤

#### Linux/macOS

```bash
# 创建构建目录
mkdir -p build && cd build

# 配置项目
cmake ..

# 编译
make -j$(nproc)

# 运行
./bin/RankingSystem
```

#### Windows

```cmd
# 创建构建目录
mkdir build && cd build

# 配置项目（使用Visual Studio生成器）
cmake .. -G "Visual Studio 16 2019" -A x64

# 编译
cmake --build . --config Release

# 运行
.\bin\Release\RankingSystem.exe
```

## 开发工作流

### 版本控制

```bash
# 克隆项目
git clone <repository-url>
cd RankingSystem

# 创建开发分支
git checkout -b feature/new-feature

# 提交更改
git add .
git commit -m "Add new feature"

# 推送分支
git push origin feature/new-feature
```

### 代码规范

#### C++ 编码规范

1. **命名约定**：
   ```cpp
   // 类名：PascalCase
   class TeamData {};
   
   // 函数名：camelCase
   void loadData();
   
   // 变量名：camelCase
   int totalScore;
   
   // 常量：UPPER_CASE
   const int MAX_TEAMS = 100;
   
   // 成员变量：m_前缀
   private:
       QString m_teamName;
   ```

2. **代码格式**：
   ```cpp
   // 使用4个空格缩进
   if (condition) {
       doSomething();
   }
   
   // 大括号换行（类和函数）
   class MyClass
   {
   public:
       void myFunction()
       {
           // 函数体
       }
   };
   ```

3. **注释规范**：
   ```cpp
   /**
    * @brief 计算队伍准确率
    * @param submissions 提交记录列表
    * @return 准确率百分比（0-100）
    */
   double calculateAccuracy(const QList<Submission>& submissions);
   
   // 单行注释用于解释代码逻辑
   int score = 0; // 初始化分数
   ```

### 测试开发

#### 单元测试

使用Qt Test框架编写测试：

```cpp
// tests/test_teamdata.cpp
#include <QtTest>
#include "../src/teamdata.h"

class TestTeamData : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();
    void testLoadFromFile();
    void testCalculateScore();
    void testDataIntegrity();

private:
    TeamData* testTeam;
};

void TestTeamData::initTestCase()
{
    testTeam = new TeamData();
}

void TestTeamData::cleanupTestCase()
{
    delete testTeam;
}

void TestTeamData::testLoadFromFile()
{
    QVERIFY(testTeam->loadFromFile("test_data.json"));
    QCOMPARE(testTeam->teamName(), "Test Team");
    QCOMPARE(testTeam->totalScore(), 100);
}

void TestTeamData::testCalculateScore()
{
    Submission submission;
    submission.score = 50;
    testTeam->addSubmission(submission);
    
    QCOMPARE(testTeam->totalScore(), 50);
}

void TestTeamData::testDataIntegrity()
{
    QString hash = testTeam->calculateHash();
    QVERIFY(!hash.isEmpty());
    QVERIFY(testTeam->verifyHash(hash));
}

#include "test_teamdata.moc"
QTEST_MAIN(TestTeamData)
```

#### 运行测试

```bash
cd build
make test
# 或者
ctest --output-on-failure
```

## 调试技巧

### Qt Creator 调试

1. **设置断点**：在代码行号左侧点击
2. **变量监视**：在Locals和Watchers窗口查看变量值
3. **调用栈**：查看函数调用路径
4. **内存分析**：使用Valgrind检查内存泄漏

### 日志调试

```cpp
#include <QDebug>
#include <QLoggingCategory>

// 定义日志类别
Q_LOGGING_CATEGORY(dataManager, "app.datamanager")

// 使用日志
qCDebug(dataManager) << "Loading team data:" << teamName;
qCWarning(dataManager) << "Invalid data format in file:" << filePath;
qCCritical(dataManager) << "Critical error:" << error;
```

### 性能分析

```cpp
#include <QElapsedTimer>

void DataManager::loadData()
{
    QElapsedTimer timer;
    timer.start();
    
    // 执行数据加载
    doLoadData();
    
    qDebug() << "Data loading took" << timer.elapsed() << "ms";
}
```

## 代码贡献

### 提交指南

1. **提交信息格式**：
   ```
   类型(范围): 简短描述
   
   详细描述（可选）
   
   相关Issue: #123
   ```

2. **提交类型**：
   - `feat`: 新功能
   - `fix`: 错误修复
   - `docs`: 文档更新
   - `style`: 代码格式化
   - `refactor`: 代码重构
   - `test`: 测试相关
   - `chore`: 构建过程或辅助工具的变动

3. **提交示例**：
   ```
   feat(charts): add new chart types support
   
   - Add radar chart implementation
   - Add bubble chart implementation
   - Update chart selection UI
   
   Fixes: #45
   ```

### 代码审查

1. **创建Pull Request**
2. **代码审查检查点**：
   - 代码规范遵循
   - 测试覆盖率
   - 文档更新
   - 性能影响
   - 安全考虑

### 版本发布

1. **版本号规则**：遵循语义化版本（Semantic Versioning）
   - MAJOR.MINOR.PATCH
   - 例如：1.0.0, 1.1.0, 1.1.1

2. **发布流程**：
   ```bash
   # 更新版本号
   git tag v1.1.0
   
   # 生成发布包
   ./scripts/package_all.sh
   
   # 推送标签
   git push origin v1.1.0
   ```

## 常见问题

### 编译问题

1. **Qt找不到**：
   ```bash
   # 设置Qt路径
   export CMAKE_PREFIX_PATH=/path/to/qt5
   ```

2. **Charts模块缺失**：
   ```bash
   # Ubuntu
   sudo apt install libqt5charts5-dev
   
   # macOS
   brew install qt@5
   ```

3. **C++17支持**：
   确保编译器支持C++17标准

### 运行时问题

1. **动态库缺失**：
   ```bash
   # Linux
   ldd ./RankingSystem
   
   # macOS  
   otool -L ./RankingSystem
   ```

2. **数据文件路径**：
   确保数据文件路径正确，使用绝对路径进行测试

### 调试技巧

1. **启用详细日志**：
   ```bash
   export QT_LOGGING_RULES="*.debug=true"
   ./RankingSystem
   ```

2. **内存检查**：
   ```bash
   valgrind --leak-check=full ./RankingSystem
   ```

## 开发工具推荐

### IDE和编辑器

1. **Qt Creator**（推荐）
   - 专为Qt开发设计
   - 集成调试器和分析工具
   - 自动代码补全

2. **Visual Studio Code**
   - 轻量级，插件丰富
   - 良好的Git集成
   - 支持远程开发

3. **CLion**
   - 强大的代码分析
   - 优秀的重构功能
   - 集成CMake支持

### 调试工具

1. **GDB/LLDB**：命令行调试器
2. **Valgrind**：内存分析工具
3. **Dr. Memory**：Windows内存检查工具
4. **Application Verifier**：Windows应用程序验证

### 性能分析

1. **Perf**：Linux性能分析工具
2. **Instruments**：macOS性能分析工具
3. **Intel VTune**：跨平台性能分析器
4. **Qt Creator Profiler**：集成性能分析

## 📝 版本控制

项目使用 Git 进行版本控制，详细配置请参考：

### 相关文档
- [版本控制指南](version_control.md) - 完整的 Git 配置和最佳实践
- [数据结构与算法](data_structures_optimized.md) - 系统中使用的数据结构详解
- [系统架构与网络功能](architecture_integrated.md) - 了解系统整体设计
- [API 参考](api_reference.md) - 查看接口文档

### 快速开始
```bash
# 检查 .gitignore 状态
./scripts/tools/manage_gitignore.sh check

# 提交代码
git add .
git commit -m "feat(ui): 添加新功能"
git push origin main
```

---

**开发指南版本：** v1.0  
**适用项目版本：** RankingSystem v1.0+  
**最后更新：** 2025年6月30日  
**维护团队：** 开发组
