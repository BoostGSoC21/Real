cmake_minimum_required(VERSION 3.8)
set(_version 1.0)
project(Boost.Real VERSION ${_version} LANGUAGES CXX)

#Add coverage support to CMAKE in debug mode
if(CMAKE_BUILD_TYPE MATCHES Debug)
    message("debug mode adding coverage report tools and flags")
    list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake-modules")
    include(CodeCoverage)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g -O0 -fsanitize=address -fno-omit-frame-pointer -fprofile-arcs -ftest-coverage")
endif(CMAKE_BUILD_TYPE MATCHES Debug)

# add Boost.Real as a 'linkable' target
add_library(Boost.Real PUBLIC)
# Check for standard to use
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag(-std=c++17 HAVE_FLAG_STD_CXX17)
target_compile_features(Boost.Real PUBLIC cxx_std_17)

include_directories(include external/include)



#Library Headers
add_executable(Boost.Real_headers include)
set_target_properties(Boost.Real_headers PROPERTIES
                                                 EXCLUDE_FROM_ALL TRUE
                                                 EXCLUDE_FROM_DEFAULT_BUILD TRUE
                                                 LINKER_LANGUAGE CXX 
                                                 )
# Unit tests
option(REAL_TEST "Build tests" ON)
if(REAL_TEST)
    enable_testing()
    #Detect Boost.Test framework
    set(Boost_USE_MULTITHREADED OFF)
    find_package(Boost COMPONENTS unit_test_framework REQUIRED)
    add_subdirectory(test)
endif()

#Google Benchmark, turn on/off with -DREAL_BENCH=ON or OFF
option(REAL_BENCH "Build benchmarks" OFF)
if(REAL_BENCH)
    add_subdirectory(bench)
endif()
