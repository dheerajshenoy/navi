# ensure cmake version is at least 3.16.0
cmake_minimum_required(VERSION 3.16.0)

# defines a project with a version
project(navifm VERSION 1.0.0 LANGUAGES CXX)

# pick the C++ standard to use, in this case C++20
set(CMAKE_CXX_STANDARD 20 CACHE STRING "C++ standard version")
set(CMAKE_CXX_STANDARD_REQUIRED ON CACHE BOOL "C++ standard required")

set(CMAKE_THREAD_LIBS_INIT "-lpthread")
set(CMAKE_HAVE_THREADS_LIBRARY 1)
set(CMAKE_USE_PTHREADS_INIT 1)
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_program(SCCACHE sccache)
find_program(CCACHE ccache)

if(CCACHE)
  set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE} CACHE STRING "CCache program for C")
  set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE} CACHE STRING "CCache program for C++")
elseif(SCCACHE)
  set(CMAKE_C_COMPILER_LAUNCHER ${SCCACHE} CACHE STRING "SCCache program for C")
  set(CMAKE_CXX_COMPILER_LAUNCHER ${SCCACHE} CACHE STRING "SCCache program for C++")
endif()


set(CMAKE_INSTALL_PREFIX "/usr" CACHE PATH "Install path prefix" FORCE)

# Define the target directory for shared files
set(SHAREDIR "${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}" CACHE PATH "/usr/share/ folder")

# Enable verbose output during build
set(CMAKE_VERBOSE_MAKEFILE ON CACHE BOOL "Verbose Makefile")

# Export compile_commands.json for IDE integration
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "Export compile commands")

# -O2
# Set flags for compilers

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
  message(STATUS "Building in Debug Mode")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g -Og -flto" CACHE STRING "C++ flags")  # Add more flags as necessary
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
  message(STATUS "Building in Release Mode")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3" CACHE STRING "C++ flags")  # Add more flags as necessary
else()
  message(STATUS "Building in Debug Mode since no build mode was specified")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build Type" FORCE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -g -Og -flto" CACHE STRING "C++ flags")  # Add more flags as necessary
endif()

# tell CMake to run the Qt tools moc, rcc, and uic automatically
set(CMAKE_AUTOMOC ON CACHE BOOL "CMAKE_AUTOMOC")
set(CMAKE_AUTORCC ON CACHE BOOL "CMAKE_AUTORCC")
set(CMAKE_AUTOUIC ON CACHE BOOL "CMAKE_AUTOUIC")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin CACHE PATH "CMAKE runtime output directory")

add_definitions( -DMAGICKCORE_QUANTUM_DEPTH=16 )
add_definitions( -DMAGICKCORE_HDRI_ENABLE=0 )

find_package(PkgConfig REQUIRED)
pkg_check_modules(POPPLER REQUIRED poppler poppler-cpp)
pkg_check_modules(LIBARCHIVE REQUIRED libarchive)
find_package(Qt6 COMPONENTS Core Widgets Network REQUIRED)
find_package(ImageMagick COMPONENTS Magick++ REQUIRED)
find_package(Lua REQUIRED)

execute_process(
  COMMAND git submodule update --init --recursive
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)

set(KDDockWidgets_QT6 "ON" CACHE STRING "Use qt6 with KDDockWidgets")
set(KDDockWidgets_FRONTENDS "qtwidgets" CACHE STRING "Use KDDockWidgets for QtWidgets only")
set(CMAKE_INSTALL_PREFIX "/usr/share/include" CACHE STRING "KDDockWidgets install prefix")

add_subdirectory(KDDockWidgets)

find_package(KDDockWidgets-qt6 REQUIRED)

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
  src/CustomInputDialog.cpp
  src/CollapsibleWidget.cpp
  src/NotificationDialog.cpp
  src/NotificationManager.cpp

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
  src/QuestionDialog.hpp
  src/NotificationDialog.hpp
  src/NotificationManager.hpp
)

# tell cmake to link the executable to the Qt 6 core and test modules

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Widgets Qt6::Network ${ImageMagick_LIBRARIES} ${LUA_LIBRARIES} poppler poppler-cpp poppler-qt6 ${LIBARCHIVE_LIBRARIES} KDAB::kddockwidgets)

set(RESOURCE_FILES
  resources/images/clock.svg
  resources/images/location.svg
  resources/images/messages.svg
  resources/images/pencil-icon.svg
  resources/images/plus.svg
  resources/images/reverse.svg
  resources/images/sort.svg
  resources/images/menu.svg
  resources/images/preview.svg
  resources/images/update.svg
)

qt_add_resources(${PROJECT_NAME}
  PREFIX "resources"
  FILES ${RESOURCE_FILES})

# Include directories
target_include_directories(${PROJECT_NAME} PRIVATE ${ImageMagick_INCLUDE_DIRS} ${LUA_INCLUDE_DIR} ${LIBARCHIVE_INCLUDE_DIRS})

add_custom_target(run
  COMMAND ${PROJECT_NAME}
  DEPENDS ${PROJECT_NAME}
  WORKING_DIRECTORY ${CMAKE_PROJECT_DIR}
)

set_target_properties(${PROJECT_NAME} PROPERTIES CMAKE_UNITY_BUILD ON)

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
target_precompile_headers(${PROJECT_NAME} PRIVATE src/pch/pch_questiondialog.hpp)

# Install the executable
install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

# Install the data folder to ${PREFIX_DIR}
install(DIRECTORY data/ DESTINATION ${SHAREDIR} COMPONENT Runtime)

install(FILES resources/navi.desktop DESTINATION ${CMAKE_INSTALL_PREFIX}/share/applications PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)

# install(FILES resources/navi.svg DESTINATION ${CMAKE_INSTALL_PREFIX}/share/icons/hicolor/scalable/apps PERMISSIONS OWNER_READ OWNER_WRITE GROUP_READ WORLD_READ)
