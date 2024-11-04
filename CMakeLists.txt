# ensure cmake version is at least 3.16.0
cmake_minimum_required(VERSION 3.16.0)

# defines a project with a version
project(Navi VERSION 1.0.0 LANGUAGES CXX)

# pick the C++ standard to use, in this case C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# tell CMake to run the Qt tools moc, rcc, and uic automatically
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)


add_definitions( -DMAGICKCORE_QUANTUM_DEPTH=16 )
add_definitions( -DMAGICKCORE_HDRI_ENABLE=0 )

find_package(Qt6 COMPONENTS Core REQUIRED)
find_package(Qt6 COMPONENTS Widgets REQUIRED)
find_package(ImageMagick COMPONENTS Magick++ REQUIRED)
find_package(Lua REQUIRED)

qt_standard_project_setup()
# define an executable built from a source file
qt_add_executable(${PROJECT_NAME}
    src/main.cpp
    src/Navi.cpp
    src/FilePanel.cpp
    src/PreviewPanel.cpp
    src/TableView.cpp
    src/FilePathWidget.cpp
    src/Statusbar.cpp
    src/Inputbar.cpp
    src/utils.cpp
    src/TreeSitterTextEdit.cpp
    src/FilePropertyWidget.cpp
    src/BookmarkManager.cpp
    src/BookmarkWidget.cpp

    src/FileWorker.hpp
    src/FileSystemModel.cpp
    src/MessagesBuffer.hpp
    src/FilePreviewWorker.hpp
    src/MarksBuffer.hpp
    src/TableHeaderView.hpp
    src/HeaderProxyStyle.hpp
)

# tell cmake to link the executable to the Qt 6 core and test modules
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Widgets ${ImageMagick_LIBRARIES} tree-sitter tree-sitter-python ${LUA_LIBRARIES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE ${ImageMagick_INCLUDE_DIRS} ${LUA_INCLUDE_DIR})

add_custom_target(run
    COMMAND Navi
    DEPENDS Navi
    WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)