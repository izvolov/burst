add_executable(kary k_ary_search_set.cpp)
target_link_libraries(kary PRIVATE Burst::burst benchIO Boost::program_options)

add_executable(dyntuple dynamic_tuple.cpp)
target_link_libraries(dyntuple PRIVATE Burst::burst benchIO Boost::program_options)

configure_file(dynamic_tuple_benchmark.py.in dynamic_tuple_benchmark.py @ONLY)
