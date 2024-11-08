# ensure cmake version is at least 3.16.0
cmake_minimum_required(VERSION 3.16.0)

# defines a project with a version
project(navi VERSION 1.0.0 LANGUAGES CXX)

# pick the C++ standard to use, in this case C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set installation prefix (default is /usr/local)
set(CMAKE_INSTALL_PREFIX "/usr/")

# Enable verbose output during build
set(CMAKE_VERBOSE_MAKEFILE ON)

# Build type of Release/Debug
set(CMAKE_BUILD_TYPE "Release")

# Export compile_commands.json for IDE integration
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Set flags for compilers
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2")  # Add more flags as necessary

# tell CMake to run the Qt tools moc, rcc, and uic automatically
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_definitions( -DMAGICKCORE_QUANTUM_DEPTH=16 )
add_definitions( -DMAGICKCORE_HDRI_ENABLE=0 )

find_package(PkgConfig REQUIRED)
pkg_check_modules(POPPLER REQUIRED poppler poppler-cpp)
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
  src/ShortcutsWidget.cpp
  src/NaviWindow.cpp
  src/DriveWidget.cpp

  src/FileWorker.hpp
  src/FileSystemModel.cpp
  src/MessagesBuffer.hpp
  src/FilePreviewWorker.hpp
  src/MarksBuffer.hpp
  src/TableHeaderView.hpp
  src/VerticalHeaderView.hpp
  src/argparse.hpp
  src/TabBarWidget.hpp
  src/StorageDevice.hpp
)

# tell cmake to link the executable to the Qt 6 core and test modules
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Widgets ${ImageMagick_LIBRARIES} tree-sitter tree-sitter-python ${LUA_LIBRARIES} poppler poppler-cpp poppler-qt6)

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE ${ImageMagick_INCLUDE_DIRS} ${LUA_INCLUDE_DIR})

add_custom_target(run
  COMMAND ${PROJECT_NAME}
  DEPENDS ${PROJECT_NAME}
  WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)

# Install the executable
install(TARGETS ${PROJECT_NAME}
  RUNTIME DESTINATION bin)