find_package(Boost 1.66.0 COMPONENTS program_options system REQUIRED)

add_library(benchIO INTERFACE)
target_include_directories(benchIO INTERFACE include)

add_subdirectory(burst)
add_subdirectory(tools)
