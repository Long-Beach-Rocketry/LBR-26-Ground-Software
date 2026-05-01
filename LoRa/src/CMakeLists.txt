add_library(cli
    cli/args.cc
    cli/config.cc
)
target_include_directories(cli PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(cli PUBLIC yaml-cpp::yaml-cpp)
lbr_enable_coverage(cli)

add_library(lora_periph
    periph/sx1262_module.cc
    periph/sx127_module.cc
)
target_include_directories(lora_periph PUBLIC ${PROJECT_SOURCE_DIR}/include)
lbr_enable_coverage(lora_periph)

add_library(connector
    connector/message.cc
    connector/ndjson_file.cc
    connector/local_tcp_transport.cc
    connector/local_udp_transport.cc
    connector/local_zmq_transport.cc
)
target_include_directories(connector PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(connector PRIVATE yaml-cpp::yaml-cpp)

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
    telemetry/interpreter.cc
    telemetry/simulation_payloads.cc
    telemetry/decoded_telemetry_publisher.cc
    telemetry/decoded_telemetry_serializer.cc
)
target_include_directories(telemetry PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(telemetry PUBLIC connector)
lbr_enable_coverage(telemetry)

add_library(sdr_pipeline
    sdr_pipeline.cc
)
target_include_directories(sdr_pipeline PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_link_libraries(sdr_pipeline PUBLIC cli telemetry)
lbr_enable_coverage(sdr_pipeline)

add_executable(lbr_ground
    main.cc
)
set_target_properties(lbr_ground PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/LoRa")
target_link_libraries(lbr_ground PRIVATE sdr_pipeline lora_periph)
lbr_enable_coverage(lbr_ground)
lbr_copy_gnu_runtime_dlls(lbr_ground)

# Export the lbr_ground target path for tests to use
set(LBR_GROUND_EXE $<TARGET_FILE:lbr_ground> PARENT_SCOPE)

add_executable(lbr_hil_runner
    hil_runner.cc
)
target_link_libraries(lbr_hil_runner PRIVATE lora_periph)
lbr_enable_coverage(lbr_hil_runner)
lbr_copy_gnu_runtime_dlls(lbr_hil_runner)
