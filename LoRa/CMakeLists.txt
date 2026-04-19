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
option(LBR_ENABLE_CLANG_TIDY "Enable clang-tidy while compiling targets" OFF)
option(LBR_ENABLE_ZEROMQ "Enable ZeroMQ connector transport" OFF)

if(CMAKE_VERSION VERSION_GREATER_EQUAL "4.0")
    set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
endif()

find_program(CLANG_FORMAT_EXECUTABLE clang-format)
find_program(CLANG_TIDY_EXECUTABLE clang-tidy)

if(LBR_ENABLE_CLANG_TIDY)
    if(NOT CLANG_TIDY_EXECUTABLE)
        message(FATAL_ERROR "LBR_ENABLE_CLANG_TIDY=ON requires clang-tidy in PATH.")
    endif()
    set(CMAKE_CXX_CLANG_TIDY ${CLANG_TIDY_EXECUTABLE})
endif()

file(GLOB_RECURSE LBR_CLANG_SOURCE_FILES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cc
)

if(CLANG_FORMAT_EXECUTABLE)
    add_custom_target(clang-format
        COMMAND ${CLANG_FORMAT_EXECUTABLE} -i ${LBR_CLANG_SOURCE_FILES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Formatting C++ sources with clang-format."
    )

    add_custom_target(clang-format-check
        COMMAND ${CLANG_FORMAT_EXECUTABLE} --dry-run --Werror ${LBR_CLANG_SOURCE_FILES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Checking C++ format with clang-format."
    )
else()
    add_custom_target(clang-format
        COMMAND ${CMAKE_COMMAND} -E echo "clang-format not found in PATH."
        COMMAND ${CMAKE_COMMAND} -E false
        COMMENT "clang-format is required for this target."
    )

    add_custom_target(clang-format-check
        COMMAND ${CMAKE_COMMAND} -E echo "clang-format not found in PATH."
        COMMAND ${CMAKE_COMMAND} -E false
        COMMENT "clang-format is required for this target."
    )
endif()

if(CLANG_TIDY_EXECUTABLE)
    add_custom_target(clang-tidy
        COMMAND ${CLANG_TIDY_EXECUTABLE} -p ${CMAKE_BINARY_DIR} ${LBR_CLANG_SOURCE_FILES}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Running clang-tidy static analysis."
    )
else()
    add_custom_target(clang-tidy
        COMMAND ${CMAKE_COMMAND} -E echo "clang-tidy not found in PATH."
        COMMAND ${CMAKE_COMMAND} -E false
        COMMENT "clang-tidy is required for this target."
    )
endif()

add_custom_target(sanity-check DEPENDS clang-format-check clang-tidy)

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
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
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

function(lbr_copy_gnu_runtime_dlls target_name)
    if(WIN32 AND CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        get_filename_component(_gcc_bin_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
        foreach(_runtime_dll libstdc++-6.dll libgcc_s_seh-1.dll libwinpthread-1.dll)
            if(EXISTS "${_gcc_bin_dir}/${_runtime_dll}")
                add_custom_command(TARGET ${target_name} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        "${_gcc_bin_dir}/${_runtime_dll}"
                        "$<TARGET_FILE_DIR:${target_name}>/${_runtime_dll}"
                )
            endif()
        endforeach()
    endif()
endfunction()

add_library(cli
    src/cli/args.cc
    src/cli/config.cc
)
target_include_directories(cli PUBLIC include)
target_link_libraries(cli PUBLIC yaml-cpp::yaml-cpp)
lbr_enable_coverage(cli)

add_library(lora_periph
    src/periph/sx1262_module.cc
    src/periph/sx127_module.cc
)
target_include_directories(lora_periph PUBLIC include)
lbr_enable_coverage(lora_periph)

add_library(connector
    src/connector/message.cc
    src/connector/ndjson_file.cc
    src/connector/local_tcp_transport.cc
    src/connector/local_udp_transport.cc
    src/connector/local_zmq_transport.cc
)
target_include_directories(connector PUBLIC include)

set(LBR_HAS_ZEROMQ 0)
if(LBR_ENABLE_ZEROMQ)
    find_path(ZMQ_INCLUDE_DIR zmq.h)
    find_library(ZMQ_LIBRARY NAMES zmq libzmq)
    if(NOT ZMQ_INCLUDE_DIR OR NOT ZMQ_LIBRARY)
        message(FATAL_ERROR "LBR_ENABLE_ZEROMQ=ON requires ZeroMQ (zmq.h + libzmq).")
    endif()

    target_include_directories(connector PRIVATE ${ZMQ_INCLUDE_DIR})
    target_link_libraries(connector PRIVATE ${ZMQ_LIBRARY})
    set(LBR_HAS_ZEROMQ 1)
endif()

target_compile_definitions(connector PUBLIC LBR_HAS_ZEROMQ=${LBR_HAS_ZEROMQ})

if(WIN32)
    target_link_libraries(connector PRIVATE ws2_32)
endif()
lbr_enable_coverage(connector)

add_library(telemetry
    src/telemetry/interpreter.cc
)
target_include_directories(telemetry PUBLIC include)
lbr_enable_coverage(telemetry)

add_library(sdr_pipeline
    src/sdr_pipeline.cc
)
target_include_directories(sdr_pipeline PUBLIC include)
target_link_libraries(sdr_pipeline PUBLIC cli telemetry)
lbr_enable_coverage(sdr_pipeline)

add_executable(lbr_ground
    src/main.cc
)
target_link_libraries(lbr_ground PRIVATE sdr_pipeline lora_periph)
lbr_enable_coverage(lbr_ground)
lbr_copy_gnu_runtime_dlls(lbr_ground)

if(BUILD_TESTING)
    include(GoogleTest)

    add_executable(lbr_tests
        tests/test_main.cc
        tests/args_tests.cc
        tests/config_tests.cc
        tests/connector_tests.cc
        tests/pipeline_tests.cc
    )
    target_include_directories(lbr_tests PRIVATE include tests)
    target_link_libraries(lbr_tests PRIVATE cli connector lora_periph sdr_pipeline GTest::gtest)
    lbr_enable_coverage(lbr_tests)
    lbr_copy_gnu_runtime_dlls(lbr_tests)

    gtest_discover_tests(lbr_tests)

    add_test(NAME app_help COMMAND lbr_ground --help)
    set_tests_properties(app_help PROPERTIES PASS_REGULAR_EXPRESSION "Usage:")

    add_test(
        NAME app_demo_config
        COMMAND lbr_ground -c ${CMAKE_CURRENT_SOURCE_DIR}/config.demo.yaml
    )
    set_tests_properties(app_demo_config PROPERTIES PASS_REGULAR_EXPRESSION "Starting SDR pipeline")

    add_test(
        NAME app_sx127
        COMMAND lbr_ground --lora-module sx127 -v
    )
    set_tests_properties(app_sx127 PROPERTIES PASS_REGULAR_EXPRESSION "lora_module: sx127")

    add_test(
        NAME app_missing_config
        COMMAND lbr_ground -c ${CMAKE_CURRENT_SOURCE_DIR}/missing-config.yaml
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
            COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_BINARY_DIR}/coverage ${GCOV_EXECUTABLE} -f -b -c -o ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/cli.dir/src/cli ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/cli.dir/src/cli/args.cc.obj ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/cli.dir/src/cli/config.cc.obj
            COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_BINARY_DIR}/coverage ${GCOV_EXECUTABLE} -f -b -c -o ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/sdr_pipeline.dir/src ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/sdr_pipeline.dir/src/sdr_pipeline.cc.obj
            COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_BINARY_DIR}/coverage ${GCOV_EXECUTABLE} -f -b -c -o ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/lbr_ground.dir/src ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/lbr_ground.dir/src/main.cc.obj
            COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_BINARY_DIR}/coverage ${GCOV_EXECUTABLE} -f -b -c -o ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/connector.dir/src/connector ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/connector.dir/src/connector/message.cc.obj ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/connector.dir/src/connector/ndjson_file.cc.obj ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/connector.dir/src/connector/local_tcp_transport.cc.obj ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/connector.dir/src/connector/local_udp_transport.cc.obj
            COMMAND ${CMAKE_COMMAND} -E chdir ${CMAKE_BINARY_DIR}/coverage ${GCOV_EXECUTABLE} -f -b -c -o ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/lora_periph.dir/src/periph ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/lora_periph.dir/src/periph/sx1262_module.cc.obj ${CMAKE_CURRENT_BINARY_DIR}/CMakeFiles/lora_periph.dir/src/periph/sx127_module.cc.obj
            WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
            COMMENT "Running tests and generating gcov coverage reports."
        )
    endif()
endif()
