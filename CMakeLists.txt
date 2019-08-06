###################################################################################################
##
##      Информация о проекте
##
###################################################################################################

cmake_minimum_required(VERSION 3.8.2)
project(Burst VERSION 1.0.0 LANGUAGES CXX)

get_directory_property(IS_SUBPROJECT PARENT_DIRECTORY)

###################################################################################################
##
##      Опции проекта
##
###################################################################################################

option(BURST_COVERAGE "Включить измерение покрытия кода тестами" OFF)
option(BURST_TESTING "Включить модульное тестирование" ON)
option(BURST_BENCHMARKING "Включить сборку программ для замеров производительности" ON)

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
##      Установка
##
###################################################################################################

install(DIRECTORY include/burst DESTINATION include)

install(TARGETS burst EXPORT BurstConfig)
install(EXPORT BurstConfig NAMESPACE Burst:: DESTINATION share/Burst/cmake)

include(CMakePackageConfigHelpers)
write_basic_package_version_file("${PROJECT_BINARY_DIR}/BurstConfigVersion.cmake"
    VERSION
        ${PROJECT_VERSION}
    COMPATIBILITY
        AnyNewerVersion
)
install(FILES "${PROJECT_BINARY_DIR}/BurstConfigVersion.cmake" DESTINATION share/Burst/cmake)

###################################################################################################
##
##      Тесты
##
###################################################################################################

if(NOT BURST_TESTING)
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

if(NOT BURST_BENCHMARKING)
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

if(NOT IS_SUBPROJECT)
    add_subdirectory(doc)
endif()

###################################################################################################
##
##      Онлайн-версия
##
###################################################################################################

if(NOT IS_SUBPROJECT)
    add_subdirectory(online)
endif()
