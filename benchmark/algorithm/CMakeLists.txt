set(GALLOPING_LOWER_BOUND_SOURCES galloping_lower_bound.cpp)
set(GALLOPING_LOWER_BOUND_EXECUTABLE gallopinglb)
add_executable(${GALLOPING_LOWER_BOUND_EXECUTABLE} ${GALLOPING_LOWER_BOUND_SOURCES})
target_link_libraries(${GALLOPING_LOWER_BOUND_EXECUTABLE} ${Boost_LIBRARIES})

set(RADIX_SORT_SOURCES radix_sort.cpp)
set(RADIX_SORT_EXECUTABLE radix)
add_executable(${RADIX_SORT_EXECUTABLE} ${RADIX_SORT_SOURCES})
target_link_libraries(${RADIX_SORT_EXECUTABLE} ${Boost_LIBRARIES})

configure_file(${CMAKE_SOURCE_DIR}/benchmark/algorithm/integer_sort_comparison.py.in integer_sort_comparison.py @ONLY)
