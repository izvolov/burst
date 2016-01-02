include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(UNIT_TEST_SOURCES test_main.cpp
    algorithm/galloping_lower_bound.cpp
    algorithm/galloping_upper_bound.cpp
    algorithm/searching/bitap.cpp
    algorithm/searching/element_position_bitmask_table.cpp
    algorithm/sorting/counting_sort.cpp
    algorithm/sorting/radix_sort.cpp
    algorithm/sum.cpp
    container/access/back.cpp
    container/access/cback.cpp
    container/access/cfront.cpp
    container/access/front.cpp
    container/dynamic_tuple.cpp
    container/k_ary_search_set.cpp
    functional/low_byte.cpp
    iterator/binary_stream_iterators.cpp
    iterator/bitap_iterator.cpp
    iterator/difference_iterator.cpp
    iterator/intersect_iterator.cpp
    iterator/join_iterator.cpp
    iterator/merge_iterator.cpp
    iterator/semiintersect_iterator.cpp
    iterator/subset_iterator.cpp
    iterator/take_n_iterator.cpp
    iterator/union_iterator.cpp
    math/intlog.cpp
    math/intlog2.cpp
    math/intpow.cpp
    range/adaptor/joined.cpp
    range/adaptor/taken_n.cpp
    range/bitap.cpp
    range/difference.cpp
    range/intersect.cpp
    range/join.cpp
    range/merge.cpp
    range/semiintersect.cpp
    range/skip_to_lower_bound.cpp
    range/skip_to_upper_bound.cpp
    range/subsets.cpp
    range/take_n.cpp
    range/unite.cpp
)

set(UNIT_TEST_EXECUTABLE unit-tests)

add_executable(${UNIT_TEST_EXECUTABLE} ${UNIT_TEST_SOURCES})
target_link_libraries(${UNIT_TEST_EXECUTABLE} ${Boost_LIBRARIES})

add_custom_target(check ALL COMMAND ${UNIT_TEST_EXECUTABLE} --report_level=short --color_output)
