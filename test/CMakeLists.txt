add_library(testutility INTERFACE)
target_include_directories(testutility INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

set(UNIT_TEST_EXECUTABLE burst-unit-tests)
add_executable(${UNIT_TEST_EXECUTABLE} test_main.cpp)
target_link_libraries(${UNIT_TEST_EXECUTABLE}
    PRIVATE
        testutility
        Burst::burst
        Boost::unit_test_framework
)

find_program(GCOVR_EXECUTABLE gcovr)
if(BURST_COVERAGE AND GCOVR_EXECUTABLE)
    message(STATUS "Test coverage is on")

    target_compile_options(${UNIT_TEST_EXECUTABLE} PRIVATE --coverage)
    target_link_libraries(${UNIT_TEST_EXECUTABLE} PRIVATE gcov)

    add_custom_target(coverage
        COMMAND
            ${GCOVR_EXECUTABLE}
                --root=${Burst_SOURCE_DIR}/include/
                --object-directory=${CMAKE_CURRENT_BINARY_DIR}
        DEPENDS
            check
    )
elseif(BURST_COVERAGE AND NOT GCOVR_EXECUTABLE)
    set(BURST_COVERAGE OFF)
    message(WARNING "Install gcovr to enable test coverage")
endif()

add_custom_target(check ALL COMMAND ${UNIT_TEST_EXECUTABLE} --report_level=short --color_output)

add_subdirectory(burst)
