find_package(Boost 1.71.0 COMPONENTS program_options system REQUIRED CONFIG)
find_package(Threads)

add_library(benchIO INTERFACE)
target_include_directories(benchIO INTERFACE include)

add_subdirectory(burst)
add_subdirectory(tools)
