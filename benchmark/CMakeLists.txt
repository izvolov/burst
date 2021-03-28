find_package(Boost 1.62.0 COMPONENTS program_options REQUIRED)

add_library(benchIO INTERFACE)
target_include_directories(benchIO INTERFACE include)

add_subdirectory(burst)
add_subdirectory(tools)
