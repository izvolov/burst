include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(rangegen range_generator.cpp)
target_link_libraries(rangegen PRIVATE Boost::program_options)

add_subdirectory(algorithm)
add_subdirectory(container)
add_subdirectory(integer)
add_subdirectory(iterator)
