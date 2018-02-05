add_library(benchIO INTERFACE)
target_include_directories(benchIO INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(rangegen range_generator.cpp)
target_link_libraries(rangegen PRIVATE Burst::burst benchIO Boost::program_options)

add_subdirectory(algorithm)
add_subdirectory(container)
add_subdirectory(integer)
add_subdirectory(iterator)
