cmake_minimum_required(VERSION 3.16)
project(RankingSystem VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Qt5组件
find_package(Qt5 REQUIRED COMPONENTS Core Widgets Charts Network)

# 设置Qt的自动处理
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# 源文件
set(SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/teamdata.cpp
    src/rankingmodel.cpp
    src/datamanager.cpp
    src/chartwidget.cpp
    src/problemwidget.cpp
    src/danmakuwidget.cpp
    src/binarysearchtree.cpp
    src/querydialog.cpp
)

# 头文件
set(HEADERS
    include/mainwindow.h
    include/teamdata.h
    include/rankingmodel.h
    include/datamanager.h
    include/chartwidget.h
    include/problemwidget.h
    include/danmakuwidget.h
    include/binarysearchtree.h
    include/querydialog.h
)

# 资源文件
set(RESOURCES
    resources/resources.qrc
)

# 创建可执行文件
add_executable(RankingSystem ${SOURCES} ${HEADERS} ${RESOURCES})

# 链接Qt库
target_link_libraries(RankingSystem 
    Qt5::Core 
    Qt5::Widgets 
    Qt5::Charts 
    Qt5::Network
)

# 设置包含目录
target_include_directories(RankingSystem PRIVATE 
    include
    ui
)

# 设置输出目录
set_target_properties(RankingSystem PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)
