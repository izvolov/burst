###################################################################################################
##
##      Информация о проекте
##
###################################################################################################

cmake_minimum_required(VERSION 3.8.2)
project(Burst)

###################################################################################################
##
##      Опции компиляции
##
###################################################################################################

add_compile_options(
    -Werror

    -Wall
    -Wextra
    -Wpedantic

    -Wcast-align
    -Wcast-qual
    -Wconversion
    -Wctor-dtor-privacy
    -Wenum-compare
    -Wfloat-equal
    -Wnon-virtual-dtor
    -Wold-style-cast
    -Woverloaded-virtual
    -Wredundant-decls
    -Wsign-conversion
    -Wsign-promo
)

if(NOT CMAKE_CXX_EXTENSIONS)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

if(NOT CMAKE_BUILD_TYPE)
    message(WARNING "Тип сборки не выбран (см. опцию CMAKE_BUILD_TYPE).")
else()
    message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
endif()

###################################################################################################
##
##      Основная цель
##
###################################################################################################

add_library(burst INTERFACE)
target_include_directories(burst INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(burst INTERFACE cxx_std_14)

add_library(Burst::burst ALIAS burst)

###################################################################################################
##
##      Сторонние библиотеки
##
###################################################################################################

find_package(Boost 1.62.0 COMPONENTS unit_test_framework program_options REQUIRED)

###################################################################################################
##
##      Установка
##
###################################################################################################

install(DIRECTORY include/burst DESTINATION include)

install(TARGETS burst EXPORT BurstConfig)
install(EXPORT BurstConfig NAMESPACE Burst:: DESTINATION share/Burst/cmake)

###################################################################################################
##
##      Покрытие кода тестами
##
###################################################################################################

option(BURST_ENABLE_COVERAGE "Включить измерение покрытия кода тестами" OFF)

if(BURST_ENABLE_COVERAGE)
    find_program(GCOVR_EXECUTABLE gcovr)
    if (GCOVR_EXECUTABLE)
        message(STATUS "Test coverage is on")
    else()
        set(BURST_ENABLE_COVERAGE OFF)
        message(WARNING "Install gcovr to enable test coverage")
    endif()
endif()

###################################################################################################
##
##      Тесты
##
###################################################################################################

option(BURST_DISABLE_TESTING "Отключить модульное тестирование" OFF)
get_directory_property(IS_SUBPROJECT PARENT_DIRECTORY)

if(BURST_DISABLE_TESTING)
    message(STATUS "Testing is disabled for Burst")
elseif(IS_SUBPROJECT)
    message(STATUS "Burst is not tested being a subproject")
else()
    add_subdirectory(test)
endif()

###################################################################################################
##
##      Измерения
##
###################################################################################################

option(BURST_DISABLE_BENCHMARKING "Отключить сборку программ для замеров производительности" OFF)

if(BURST_DISABLE_BENCHMARKING)
    message(STATUS "Benchmarking is disabled for Burst")
elseif(IS_SUBPROJECT)
    message(STATUS "Burst is not benchmarked being a subproject")
else()
    add_subdirectory(benchmark)
endif()

###################################################################################################
##
##      Документация
##
###################################################################################################

add_subdirectory(doc)

###################################################################################################
##
##      Онлайн-версия
##
###################################################################################################

add_subdirectory(online)
