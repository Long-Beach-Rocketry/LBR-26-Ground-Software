add_executable(lbr_tests
    test_main.cc
    args_tests.cc
    config_tests.cc
    connector_tests.cc
    pipeline_tests.cc
    telemetry_interpreter_tests.cc
    telemetry_serializer_tests.cc
)
target_include_directories(lbr_tests PRIVATE ${PROJECT_SOURCE_DIR}/include ${PROJECT_SOURCE_DIR}/tests)
target_link_libraries(lbr_tests PRIVATE cli connector lora_periph sdr_pipeline GTest::gtest)
lbr_enable_coverage(lbr_tests)
lbr_copy_gnu_runtime_dlls(lbr_tests)

gtest_discover_tests(lbr_tests)

add_test(NAME app_help COMMAND $<TARGET_FILE:lbr_ground> --help)
set_tests_properties(app_help PROPERTIES PASS_REGULAR_EXPRESSION "Usage:")

add_test(NAME app_demo_config COMMAND $<TARGET_FILE:lbr_ground> -c ${PROJECT_SOURCE_DIR}/config.demo.yaml)
set_tests_properties(app_demo_config PROPERTIES PASS_REGULAR_EXPRESSION "Starting SDR pipeline")

add_test(NAME app_sx127 COMMAND $<TARGET_FILE:lbr_ground> --lora-module sx127 -v)
set_tests_properties(app_sx127 PROPERTIES PASS_REGULAR_EXPRESSION "lora_module: sx127")

add_test(NAME app_missing_config COMMAND $<TARGET_FILE:lbr_ground> -c ${PROJECT_SOURCE_DIR}/missing-config.yaml)
set_tests_properties(app_missing_config PROPERTIES WILL_FAIL TRUE)
