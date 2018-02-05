add_executable(gallopinglb galloping_lower_bound.cpp)
target_link_libraries(gallopinglb PRIVATE Boost::program_options)

add_executable(radix radix_sort.cpp)
target_link_libraries(radix PRIVATE Boost::program_options)

configure_file(integer_sort_comparison.py.in integer_sort_comparison.py @ONLY)
