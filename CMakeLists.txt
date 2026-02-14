#
# @file CMakeLists.txt
# @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
#

cmake_minimum_required(VERSION 3.20)

project(LBR26GroundSoftware VERSION 0.1.0 LANGUAGES CXX)

include(FetchContent)
include(CTest)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(LBR_ENABLE_COVERAGE "Enable gcov coverage instrumentation (GNU only)" OFF)

if(CMAKE_VERSION VERSION_GREATER_EQUAL "4.0")
    set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
endif()

if(LBR_ENABLE_COVERAGE AND NOT CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    message(FATAL_ERROR "LBR_ENABLE_COVERAGE requires GNU compiler.")
endif()

if(LBR_ENABLE_COVERAGE AND NOT BUILD_TESTING)
    message(FATAL_ERROR "LBR_ENABLE_COVERAGE requires BUILD_TESTING=ON.")
endif()

FetchContent_Declare(
    yaml-cpp
    GIT_REPOSITORY https://github.com/jbeder/yaml-cpp.git
    GIT_TAG yaml-cpp-0.9.0
)
FetchContent_MakeAvailable(yaml-cpp)

if(BUILD_TESTING)
    FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/refs/tags/v1.15.2.zip
    )
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
endif()

function(lbr_enable_coverage target_name)
    if(LBR_ENABLE_COVERAGE)
        target_compile_options(${target_name} PRIVATE -O0 -g --coverage)
        target_link_options(${target_name} PRIVATE --coverage)
    endif()
endfunction()

add_library(cli
    src/cli/args.cc
    src/cli/config.cc
)
target_include_directories(cli PUBLIC include)
target_link_libraries(cli PUBLIC yaml-cpp::yaml-cpp)
lbr_enable_coverage(cli)

add_library(sdr_pipeline
    src/sdr_pipeline.cc
)
target_include_directories(sdr_pipeline PUBLIC include)
target_link_libraries(sdr_pipeline PUBLIC cli)
lbr_enable_coverage(sdr_pipeline)

add_executable(lbr_ground
    src/main.cc
)
target_link_libraries(lbr_ground PRIVATE sdr_pipeline)
lbr_enable_coverage(lbr_ground)

if(BUILD_TESTING)
    add_executable(lbr_tests
        tests/test_main.cc
        tests/args_tests.cc
        tests/config_tests.cc
        tests/pipeline_tests.cc
    )
    target_include_directories(lbr_tests PRIVATE include tests)
    target_link_libraries(lbr_tests PRIVATE cli sdr_pipeline GTest::gtest)
    lbr_enable_coverage(lbr_tests)

    add_test(NAME unit_tests COMMAND lbr_tests)

    add_test(NAME app_help COMMAND lbr_ground --help)
    set_tests_properties(app_help PROPERTIES PASS_REGULAR_EXPRESSION "Usage:")

    add_test(
        NAME app_demo_config
        COMMAND lbr_ground -c ${CMAKE_SOURCE_DIR}/config.demo.yaml
    )
    set_tests_properties(app_demo_config PROPERTIES PASS_REGULAR_EXPRESSION "Starting SDR pipeline")

    add_test(
        NAME app_missing_config
        COMMAND lbr_ground -c ${CMAKE_SOURCE_DIR}/missing-config.yaml
    )
    set_tests_properties(app_missing_config PROPERTIES WILL_FAIL TRUE)
endif()

if(LBR_ENABLE_COVERAGE)
    find_program(GCOV_EXECUTABLE gcov PATHS C:/msys64/ucrt64/bin)
    if(NOT GCOV_EXECUTABLE)
        message(WARNING "gcov not found. Coverage target will not be available.")
    else()
        add_custom_target(
            coverage
            COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
            COMMAND ${CMAKE_COMMAND} -E rm -rf ${CMAKE_BINARY_DIR}/coverage
            COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/coverage
            COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_BINARY_DIR}/coverage ${GCOV_EXECUTABLE} -b -c -o ${CMAKE_BINARY_DIR}/CMakeFiles/cli.dir/src/cli ${CMAKE_BINARY_DIR}/CMakeFiles/cli.dir/src/cli/args.cc.obj ${CMAKE_BINARY_DIR}/CMakeFiles/cli.dir/src/cli/config.cc.obj
            COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_BINARY_DIR}/coverage ${GCOV_EXECUTABLE} -b -c -o ${CMAKE_BINARY_DIR}/CMakeFiles/sdr_pipeline.dir/src ${CMAKE_BINARY_DIR}/CMakeFiles/sdr_pipeline.dir/src/sdr_pipeline.cc.obj
            COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_BINARY_DIR}/coverage ${GCOV_EXECUTABLE} -b -c -o ${CMAKE_BINARY_DIR}/CMakeFiles/lbr_ground.dir/src ${CMAKE_BINARY_DIR}/CMakeFiles/lbr_ground.dir/src/main.cc.obj
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Running tests and generating gcov coverage reports."
        )
    endif()
endif()
