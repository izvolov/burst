add_library(testutility INTERFACE)
target_include_directories(testutility INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

set(UNIT_TEST_EXECUTABLE unit-tests)
add_executable(${UNIT_TEST_EXECUTABLE} test_main.cpp)
target_link_libraries(${UNIT_TEST_EXECUTABLE}
    PRIVATE
        testutility
        Burst::burst
        Boost::unit_test_framework
)

add_custom_target(check ALL COMMAND ${UNIT_TEST_EXECUTABLE} --report_level=short --color_output)

add_subdirectory(burst)
