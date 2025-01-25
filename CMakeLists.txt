# ensure cmake version is at least 3.16.0
cmake_minimum_required(VERSION 3.16.0)

# defines a project with a version
project(navi VERSION 1.0.0 LANGUAGES CXX)

# pick the C++ standard to use, in this case C++20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "Install path prefix" FORCE)

# Define the target directory for shared files
set(SHAREDIR "${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}")

# Enable verbose output during build
set(CMAKE_VERBOSE_MAKEFILE ON)

# Export compile_commands.json for IDE integration
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# -O2
# Set flags for compilers

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Building in Debug Mode")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g -Og")  # Add more flags as necessary
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "Building in Release Mode")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")  # Add more flags as necessary
else()
    message(STATUS "Building in Debug Mode since no build mode was specified")
    set(CMAKE_BUILD_TYPE "Debug")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g -Og")  # Add more flags as necessary
endif()

# tell CMake to run the Qt tools moc, rcc, and uic automatically
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_definitions( -DMAGICKCORE_QUANTUM_DEPTH=16 )
add_definitions( -DMAGICKCORE_HDRI_ENABLE=0 )

find_package(PkgConfig REQUIRED)
pkg_check_modules(POPPLER REQUIRED poppler poppler-cpp)
pkg_check_modules(LIBARCHIVE REQUIRED libarchive)
find_package(Qt6 REQUIRED COMPONENTS Core)
find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(Qt6 REQUIRED COMPONENTS Network)
find_package(ImageMagick COMPONENTS Magick++ REQUIRED)
find_package(Lua REQUIRED)

qt_standard_project_setup()
# define an executable built from a source file
qt_add_executable(${PROJECT_NAME}
  src/main.cpp
  src/Navi.cpp
  src/utils.cpp
  src/FilePanel.cpp
  src/PreviewPanel.cpp
  src/TableView.cpp
  src/FilePathWidget.cpp
  src/Statusbar.cpp
  src/Inputbar.cpp
  src/FilePropertyWidget.cpp
  src/BookmarkManager.cpp
  src/BookmarkWidget.cpp
  src/ShortcutsWidget.cpp
  src/NaviWindow.cpp
  src/DriveWidget.cpp
  src/TasksWidget.cpp
  src/Task.cpp
  src/TaskManager.cpp
  src/RegisterWidget.cpp
  src/HookManager.cpp
  src/FileWorker.cpp
  src/Thumbnailer.cpp
  src/FolderPropertyWidget.cpp
  src/AboutWidget.cpp
  src/API.cpp
  src/Mimeutils.cpp
  src/DesktopFile.cpp
  src/UpdateDialog.cpp
  src/WaitingSpinnerWidget.cpp

  src/Globals.hpp
  src/TextEdit.hpp
  src/FileSystemModel.cpp
  src/MessagesBuffer.hpp
  src/MarksBuffer.hpp
  src/TableHeaderView.hpp
  src/VerticalHeaderView.hpp
  src/argparse.hpp
  src/TabBarWidget.hpp
  src/StorageDevice.hpp
  src/ImageWidget.hpp
  src/FilePreviewWorker.hpp
  src/TaskCardTemplate.hpp
  src/OrderlessFilterModel.hpp
  src/FilePanelDelegate.hpp
)


# tell cmake to link the executable to the Qt 6 core and test modules
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Widgets Qt6::Network ${ImageMagick_LIBRARIES} ${LUA_LIBRARIES} poppler poppler-cpp poppler-qt6 ${LIBARCHIVE_LIBRARIES})

qt_add_resources(${PROJECT_NAME}
  PREFIX "resources"
  FILES
  resources/images/clock.svg
  resources/images/location.svg
  resources/images/messages.svg
  resources/images/pencil-icon.svg
  resources/images/plus.svg
  resources/images/reverse.svg
  resources/images/sort.svg
  resources/images/menu.svg
  resources/images/preview.svg
)


# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE ${ImageMagick_INCLUDE_DIRS} ${LUA_INCLUDE_DIR} ${LIBARCHIVE_INCLUDE_DIRS})

add_custom_target(run
  COMMAND ${PROJECT_NAME}
  DEPENDS ${PROJECT_NAME}
  WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)

target_precompile_headers(${PROJECT_NAME} PRIVATE src/pch/pch.hpp)
target_precompile_headers(${PROJECT_NAME} PRIVATE src/pch/pch_filepanel.hpp)
target_precompile_headers(${PROJECT_NAME} PRIVATE src/pch/pch_inputbar.hpp)
target_precompile_headers(${PROJECT_NAME} PRIVATE src/pch/pch_statusbar.hpp)
target_precompile_headers(${PROJECT_NAME} PRIVATE src/pch/pch_utils.hpp)
target_precompile_headers(${PROJECT_NAME} PRIVATE src/pch/pch_filepathwidget.hpp)
target_precompile_headers(${PROJECT_NAME} PRIVATE src/pch/pch_previewpanel.hpp)
target_precompile_headers(${PROJECT_NAME} PRIVATE src/pch/pch_shortcutswidget.hpp)
target_precompile_headers(${PROJECT_NAME} PRIVATE src/pch/pch_tableview.hpp)
target_precompile_headers(${PROJECT_NAME} PRIVATE src/pch/pch_taskcardtemplate.hpp)
target_precompile_headers(${PROJECT_NAME} PRIVATE src/pch/pch_task.hpp)
target_precompile_headers(${PROJECT_NAME} PRIVATE src/pch/pch_taskswidget.hpp)
target_precompile_headers(${PROJECT_NAME} PRIVATE src/pch/pch_thumbnailer.hpp)
target_precompile_headers(${PROJECT_NAME} PRIVATE src/pch/pch_folderpropertywidget.hpp)
target_precompile_headers(${PROJECT_NAME} PRIVATE src/pch/pch_fileworker.hpp)
target_precompile_headers(${PROJECT_NAME} PRIVATE src/pch/pch_filesystemmodel.hpp)
target_precompile_headers(${PROJECT_NAME} PRIVATE src/pch/pch_filepropertywidget.hpp)
target_precompile_headers(${PROJECT_NAME} PRIVATE src/pch/pch_filepreviewworker.hpp)
target_precompile_headers(${PROJECT_NAME} PRIVATE src/pch/pch_drivewidget.hpp)
target_precompile_headers(${PROJECT_NAME} PRIVATE src/pch/pch_filepaneldelegate.hpp)
target_precompile_headers(${PROJECT_NAME} PRIVATE src/pch/pch_mimeutils.hpp)
target_precompile_headers(${PROJECT_NAME} PRIVATE src/pch/pch_updatedialog.hpp)

# Install the executable
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

# Install the data folder to ${PREFIX_DIR}
install(DIRECTORY data/ DESTINATION ${SHAREDIR} COMPONENT Runtime)
