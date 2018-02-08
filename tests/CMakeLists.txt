cmake_minimum_required(VERSION 3.1)
project(MinerProfilerTests)

set(CMAKE_CXX_STANDARD 11)

enable_testing()

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)

# BUILD TESTS

configure_file(data/portfolio_value_test_data.txt data/portfolio_value_test_data.txt COPYONLY)

set(TARGET_NAME MinerProfilerTests)

add_executable(MinerProfilerTests
        MinerProfiler_test.cpp)

target_link_libraries(MinerProfilerTests
        PRIVATE MinerProfilerLib Qt5::Test Qt5::Widgets Qt5::Core Qt5::PrintSupport)
