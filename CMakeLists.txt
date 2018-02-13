cmake_minimum_required(VERSION 3.1)
project(MinerProfilerTests)

find_package(Boost)

set(CMAKE_CXX_STANDARD 11)
set( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -fPIC")

enable_testing()

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
# add ui/ directory to search path
set(CMAKE_AUTOUIC_SEARCH_PATHS ui/)

# Find the QtWidgets libraries
find_package(Qt5 COMPONENTS Widgets PrintSupport Core Test REQUIRED)

# Create some variables with relevant files
set(MinerProfiler_UIS ui/mainwindow.ui)

set(MinerProfiler_MOC_SRCS
        include/mainwindow.h
        include/qcustomplot.h)

set(MinerProfiler_SRCS
        src/mainwindow.cpp
        src/qcustomplot.cpp)

# Create a static library with the main project
# and then use it to run the tests -> avoids recompiling
set(TARGET_NAME MinerProfilerLib)

add_library(${TARGET_NAME} STATIC
        ${MinerProfiler_SRCS}
        ${MinerProfiler_MOC_SRCS}
        ${MinerProfiler_UIS})

# BUILD PROJECT
add_executable(MinerProfiler src/main.cpp)

target_include_directories(${TARGET_NAME}
        PUBLIC
        include/
        src/
        ui/
        $<TARGET_PROPERTY:Qt5::Widgets,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:Qt5::Core,INTERFACE_INCLUDE_DIRECTORIES>
        $<TARGET_PROPERTY:Qt5::PrintSupport,INTERFACE_INCLUDE_DIRECTORIES>
        ${Boost_INCLUDE_DIR})

target_link_libraries(MinerProfiler
        MinerProfilerLib Qt5::Widgets Qt5::Core Qt5::PrintSupport ${Boost_LIBRARY_DIR})

add_subdirectory(tests)

add_test(NAME MinerProfilerParserTests COMMAND MinerProfilerParserTests
         WORKING_DIRECTORY ${MinerProfilerTests_SOURCE_DIR})

add_test(NAME MinerProfilerSMATests COMMAND MinerProfilerSMATests
         WORKING_DIRECTORY ${MinerProfilerTests_SOURCE_DIR})
