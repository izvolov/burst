find_package(Boost 1.66.0 COMPONENTS system REQUIRED)
find_package(doctest 2.3.4 REQUIRED)
find_package(Threads)

add_library(testutility INTERFACE)
target_include_directories(testutility INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(burst-unit-tests test_main.cpp)
target_link_libraries(burst-unit-tests
    PRIVATE
        Boost::boost
        Boost::system
        Burst::burst
        doctest::doctest
        testutility
        Threads::Threads
)

find_program(GCOVR_EXECUTABLE gcovr)
if(BURST_COVERAGE AND GCOVR_EXECUTABLE)
    message(STATUS "Test coverage is on")

    target_compile_options(burst-unit-tests PRIVATE --coverage)
    target_link_libraries(burst-unit-tests PRIVATE gcov)

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

add_custom_target(check ALL COMMAND burst-unit-tests --report_level=short --color_output)

add_subdirectory(burst)
