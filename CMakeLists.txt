cmake_minimum_required(VERSION 3.1)
project(MinerProfiler)

set(CMAKE_CXX_STANDARD 11)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
# add ui/ directory to search path
set(CMAKE_AUTOUIC_SEARCH_PATHS ui/)

# Find the QtWidgets libraries
find_package(Qt5 COMPONENTS Widgets PrintSupport Core REQUIRED)

include_directories(include/)

set(MinerProfiler_UIS ui/mainwindow.ui)

set(MinerProfiler_MOC_SRCS include/mainwindow.h
                           include/qcustomplot.h)

set(MinerProfiler_SRCS src/main.cpp
                       src/mainwindow.cpp
                       src/qcustomplot.cpp)

add_executable(MinerProfiler
                ${MinerProfiler_MOC_SRCS}
                ${MinerProfiler_SRCS}
                ${MinerProfiler_UIS})

# Use the Widgets, Core and PrintSupport modules from Qt 5.
qt5_use_modules(MinerProfiler Widgets Core PrintSupport)