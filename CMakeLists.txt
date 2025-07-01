cmake_minimum_required(VERSION 3.16)
project(SyntheticArbitrageEngine VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Compiler-specific options for optimization
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG -march=native")
    set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
endif()

# Enable position independent code
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)
include_directories(${CMAKE_SOURCE_DIR}/external)

# Find required packages
find_package(Threads REQUIRED)
find_package(Boost REQUIRED COMPONENTS system filesystem thread)
find_package(OpenSSL REQUIRED)

# Add custom module path for finding packages
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# Include external dependencies
include(ExternalProject)

# nlohmann/json - Header-only JSON library
ExternalProject_Add(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_SOURCE_DIR}/external/nlohmann_json
        -DJSON_BuildTests=OFF
        -DJSON_Install=ON
)

# spdlog - Fast C++ logging library
ExternalProject_Add(
    spdlog
    GIT_REPOSITORY https://github.com/gabime/spdlog.git
    GIT_TAG v1.12.0
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_SOURCE_DIR}/external/spdlog
        -DSPDLOG_BUILD_EXAMPLE=OFF
        -DSPDLOG_BUILD_TESTS=OFF
)

# Catch2 - Modern C++ test framework
ExternalProject_Add(
    catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG v3.4.0
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_SOURCE_DIR}/external/catch2
        -DCATCH_INSTALL_DOCS=OFF
        -DCATCH_INSTALL_EXTRAS=OFF
)

# WebSocket++ - Header-only WebSocket library (manual install)
ExternalProject_Add(
    websocketpp
    GIT_REPOSITORY https://github.com/zaphoyd/websocketpp.git
    GIT_TAG 0.8.2
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory 
        <SOURCE_DIR>/websocketpp 
        ${CMAKE_SOURCE_DIR}/external/websocketpp/include/websocketpp
)

# For now, we'll implement our own concurrent containers instead of TBB
# This will be sufficient for Phase 1 testing

# xsimd - SIMD intrinsics wrapper (header-only, manual install)
ExternalProject_Add(
    xsimd
    GIT_REPOSITORY https://github.com/xtensor-stack/xsimd.git
    GIT_TAG 10.0.0
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND ${CMAKE_COMMAND} -E copy_directory 
        <SOURCE_DIR>/include 
        ${CMAKE_SOURCE_DIR}/external/xsimd/include
)

# Create interface libraries for external dependencies
add_library(nlohmann_json_lib INTERFACE)
add_dependencies(nlohmann_json_lib nlohmann_json)
target_include_directories(nlohmann_json_lib INTERFACE ${CMAKE_SOURCE_DIR}/external/nlohmann_json/include)

add_library(spdlog_lib INTERFACE)
add_dependencies(spdlog_lib spdlog)
target_include_directories(spdlog_lib INTERFACE ${CMAKE_SOURCE_DIR}/external/spdlog/include)
target_link_directories(spdlog_lib INTERFACE ${CMAKE_SOURCE_DIR}/external/spdlog/lib)

add_library(catch2_lib INTERFACE)
add_dependencies(catch2_lib catch2)
target_include_directories(catch2_lib INTERFACE ${CMAKE_SOURCE_DIR}/external/catch2/include)

add_library(websocketpp_lib INTERFACE)
add_dependencies(websocketpp_lib websocketpp)
target_include_directories(websocketpp_lib INTERFACE ${CMAKE_SOURCE_DIR}/external/websocketpp/include)

add_library(tbb_lib INTERFACE)
# TBB functionality will be implemented using standard library threading for Phase 1

add_library(xsimd_lib INTERFACE)
add_dependencies(xsimd_lib xsimd)
target_include_directories(xsimd_lib INTERFACE ${CMAKE_SOURCE_DIR}/external/xsimd/include)

# Core library
add_subdirectory(src)

# Tests
enable_testing()
add_subdirectory(tests)

# Main executable
add_executable(arbitrage_engine src/main.cpp)
target_link_libraries(arbitrage_engine
    arbitrage_core
    arbitrage_data
    arbitrage_utils
    Threads::Threads
    Boost::system
    Boost::filesystem
    Boost::thread
    OpenSSL::SSL
    OpenSSL::Crypto
)

# Phase 2 demo executable
add_executable(phase2_demo src/phase2_demo.cpp)
target_link_libraries(phase2_demo
    arbitrage_data
    arbitrage_utils
    Threads::Threads
    OpenSSL::SSL
    OpenSSL::Crypto
)

# Phase 3 demo executable
add_executable(phase3_demo src/phase3_demo.cpp)
target_link_libraries(phase3_demo
    arbitrage_core
    arbitrage_data
    arbitrage_utils
    Threads::Threads
    OpenSSL::SSL
    OpenSSL::Crypto
)

# Dashboard demo executable (Phase 9)
add_executable(dashboard_demo src/dashboard_demo.cpp)
target_link_libraries(dashboard_demo
    arbitrage_ui
    arbitrage_core
    arbitrage_data
    arbitrage_utils
    Threads::Threads
    OpenSSL::SSL
    OpenSSL::Crypto
)

# Phase 4 demo executable (Arbitrage Detection Engine)
add_executable(phase4_demo src/phase4_demo.cpp)
target_link_libraries(phase4_demo
    arbitrage_core
    arbitrage_data
    arbitrage_utils
    Threads::Threads
    OpenSSL::SSL
    OpenSSL::Crypto
)

# Set output directories
set_target_properties(arbitrage_engine phase2_demo phase3_demo phase4_demo dashboard_demo PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# Install targets
install(TARGETS arbitrage_engine DESTINATION bin)
install(DIRECTORY config/ DESTINATION config)
