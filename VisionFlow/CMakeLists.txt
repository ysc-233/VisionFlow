cmake_minimum_required(VERSION 3.10)

project(VisionFlow VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# ---------------------------
# 1️⃣ Qt 配置
# --------------------------
find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Core OpenGL Gui REQUIRED)
find_package(Qt${QT_VERSION_MAJOR}
    COMPONENTS Widgets Core OpenGL Gui
    REQUIRED
)

# -------------------------
# 2️⃣ QtNodes 第三方库
# --------------------------
list(APPEND CMAKE_PREFIX_PATH
    ${CMAKE_CURRENT_SOURCE_DIR}/ThirdParties/QtNodes
)

find_package(QtNodes REQUIRED)

# --------------------------
# 3️⃣ 第三方库目录
# --------------------------
add_subdirectory(ThirdParties)

# --------------------------
# 4️⃣ 项目源文件
# --------------------------
file(GLOB_RECURSE PROJECT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/ui/*.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/*.ui
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)

list(FILTER PROJECT_SOURCES EXCLUDE REGEX ".*/ThirdParties/.*")

if(NOT PROJECT_SOURCES)
    file(WRITE ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        "#include <QApplication>\n"
        "#include <QMainWindow>\n"
        "int main(int argc, char *argv[]){\n"
        "QApplication app(argc, argv);\n"
        "QMainWindow w;\n"
        "w.show();\n"
        "return app.exec();}\n"
    )
    set(PROJECT_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
endif()

# --------------------------
# 5️⃣ 创建可执行程序
# --------------------------
add_executable(VisionFlow WIN32 ${PROJECT_SOURCES})

# --------------------------
# 6️⃣ 链接库
# --------------------------
target_link_libraries(VisionFlow PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::OpenGL
    Qt${QT_VERSION_MAJOR}::Gui
    QtNodes::QtNodes
    ${OpenCV_LIBS}
)

target_include_directories(VisionFlow PRIVATE
    ${OpenCV_INCLUDE_DIRS}
)

# --------------------------
# 7️⃣ 编译定义
# --------------------------
target_compile_definitions(VisionFlow PRIVATE
    QT_DEPRECATED_WARNINGS
)

# --------------------------
# 8️⃣ 输出目录
# --------------------------
set_target_properties(VisionFlow PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/Debug
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release
)

# --------------------------
# 9️⃣ Qt6 自动部署
# --------------------------
if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(VisionFlow)
endif()
