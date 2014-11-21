###################################################################################################
##
##      Информация о проекте
##
###################################################################################################

project(Thrust)
cmake_minimum_required(VERSION 2.8)

###################################################################################################
##
##      Опции компиляции
##
###################################################################################################

set(WARNING_FLAGS "-Wall -Wextra -Wpedantic -Wconversion -Werror")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 ${WARNING_FLAGS}")

if(NOT CMAKE_BUILD_TYPE)
    message(WARNING "Тип сборки не выбран (см. опцию CMAKE_BUILD_TYPE).")
else()
    message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
endif()

include_directories(${CMAKE_SOURCE_DIR})

###################################################################################################
##
##      Сторонние библиотеки
##
###################################################################################################

find_package (Boost 1.54.0 COMPONENTS unit_test_framework REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

###################################################################################################
##
##      Тесты
##
###################################################################################################

set(TEST_SOURCES tests/test_main.cpp
                 tests/iterator/merge_iterator.cpp)

set(TEST_EXECUTABLE tests)

add_executable(${TEST_EXECUTABLE} ${TEST_SOURCES})
target_link_libraries(${TEST_EXECUTABLE} ${Boost_LIBRARIES})

add_custom_target(check COMMAND ${TEST_EXECUTABLE} --log_level=test_suite --report_level=short)
