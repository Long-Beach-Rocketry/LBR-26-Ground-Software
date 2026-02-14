#
# @file CMakeLists.txt
# @author Luis Fernandes (luisrobertantonio.fernandes01@student.csulb.edu)
#

cmake_minimum_required(VERSION 3.20)

project(LBR26GroundSoftware VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(cli
    src/cli/args.cc
    src/cli/config.cc
)
target_include_directories(cli PUBLIC include)

add_library(sdr_pipeline
    src/sdr_pipeline.cc
)
target_include_directories(sdr_pipeline PUBLIC include)
target_link_libraries(sdr_pipeline PUBLIC cli)

add_executable(lbr_ground
    src/main.cc
)
target_link_libraries(lbr_ground PRIVATE sdr_pipeline)
