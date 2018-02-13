cmake_minimum_required(VERSION 3.1)
project(MinerProfilerTests)

set(CMAKE_CXX_STANDARD 11)

enable_testing()

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC OFF)

# BUILD TESTS

configure_file(data/portfolio_value.txt data/portfolio_value.txt COPYONLY)
configure_file(data/portfolio_value_USD.txt data/portfolio_value_USD.txt COPYONLY)
configure_file(data/price_log_us-order1.txt data/price_log_us-order1.txt COPYONLY)
configure_file(data/speed_log_us-order1.txt data/speed_log_us-order1.txt COPYONLY)

add_executable(MinerProfilerParserTests
        MinerProfilerParserTests.cpp)

target_link_libraries(MinerProfilerParserTests
        PRIVATE MinerProfilerLib Qt5::Test Qt5::Widgets Qt5::Core Qt5::PrintSupport)

add_executable(MinerProfilerSMATests
        MinerProfilerSMATests.cpp)

target_link_libraries(MinerProfilerSMATests
        PRIVATE MinerProfilerLib Qt5::Test Qt5::Widgets Qt5::Core Qt5::PrintSupport)