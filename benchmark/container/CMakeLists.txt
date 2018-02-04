set(K_ARY_SEARCH_SOURCES k_ary_search_set.cpp)
set(K_ARY_SEARCH_EXECUTABLE kary)
add_executable(${K_ARY_SEARCH_EXECUTABLE} ${K_ARY_SEARCH_SOURCES})
target_link_libraries(${K_ARY_SEARCH_EXECUTABLE} PRIVATE Boost::program_options)

set(DYNAMIC_TUPLE_SOURCES dynamic_tuple.cpp)
set(DYNAMIC_TUPLE_EXECUTABLE dyntuple)
add_executable(${DYNAMIC_TUPLE_EXECUTABLE} ${DYNAMIC_TUPLE_SOURCES})
target_link_libraries(${DYNAMIC_TUPLE_EXECUTABLE} PRIVATE Boost::program_options)

configure_file(${CMAKE_SOURCE_DIR}/benchmark/container/dynamic_tuple_benchmark.py.in dynamic_tuple_benchmark.py @ONLY)
