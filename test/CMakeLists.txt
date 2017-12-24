set(UNIT_TEST_SOURCES test_main.cpp
    burst/algorithm/counting_sort.cpp
    burst/algorithm/galloping_lower_bound.cpp
    burst/algorithm/galloping_upper_bound.cpp
    burst/algorithm/next_subset.cpp
    burst/algorithm/radix_sort.cpp
    burst/algorithm/searching/bitap.cpp
    burst/algorithm/searching/element_position_bitmask_table.cpp
    burst/algorithm/select_min.cpp
    burst/algorithm/sum.cpp
    burst/container/access/back.cpp
    burst/container/access/cback.cpp
    burst/container/access/cfront.cpp
    burst/container/access/front.cpp
    burst/container/dynamic_tuple.cpp
    burst/container/k_ary_search_set.cpp
    burst/container/make_sequence_container.cpp
    burst/container/make_set.cpp
    burst/functional/compose.cpp
    burst/functional/each.cpp
    burst/functional/fn.cpp
    burst/functional/low_byte.cpp
    burst/functional/mem_fn.cpp
    burst/functional/member.cpp
    burst/functional/part.cpp
    burst/integer/divceil.cpp
    burst/integer/intceil.cpp
    burst/integer/intlog.cpp
    burst/integer/intlog2.cpp
    burst/integer/intpow.cpp
    burst/integer/left_shift.cpp
    burst/integer/right_shift.cpp
    burst/integer/to_unsigned.cpp
    burst/iterator/binary_stream_iterators.cpp
    burst/iterator/bitap_iterator.cpp
    burst/iterator/cache_iterator.cpp
    burst/iterator/difference_iterator.cpp
    burst/iterator/intersect_iterator.cpp
    burst/iterator/join_iterator.cpp
    burst/iterator/merge_iterator.cpp
    burst/iterator/owning_iterator.cpp
    burst/iterator/semiintersect_iterator.cpp
    burst/iterator/subset_iterator.cpp
    burst/iterator/symmetric_difference_iterator.cpp
    burst/iterator/take_n_iterator.cpp
    burst/iterator/union_iterator.cpp
    burst/range/adaptor/cached_one.cpp
    burst/range/adaptor/intersected.cpp
    burst/range/adaptor/joined.cpp
    burst/range/adaptor/merged.cpp
    burst/range/adaptor/owned_as_range.cpp
    burst/range/adaptor/semiintersected.cpp
    burst/range/adaptor/symmetric_differenced.cpp
    burst/range/adaptor/taken_n.cpp
    burst/range/adaptor/united.cpp
    burst/range/bitap.cpp
    burst/range/cache_one.cpp
    burst/range/difference.cpp
    burst/range/intersect.cpp
    burst/range/join.cpp
    burst/range/merge.cpp
    burst/range/own_as_range.cpp
    burst/range/semiintersect.cpp
    burst/range/skip_to_lower_bound.cpp
    burst/range/skip_to_upper_bound.cpp
    burst/range/subsets.cpp
    burst/range/symmetric_difference.cpp
    burst/range/take_n.cpp
    burst/range/unite.cpp
    burst/tuple/apply.cpp
    burst/tuple/by.cpp
    burst/tuple/get.cpp
    burst/tuple/view.cpp
)

set(UNIT_TEST_EXECUTABLE unit-tests)

add_executable(${UNIT_TEST_EXECUTABLE} ${UNIT_TEST_SOURCES})
target_link_libraries(${UNIT_TEST_EXECUTABLE} ${Boost_LIBRARIES})

add_custom_target(check ALL COMMAND ${UNIT_TEST_EXECUTABLE} --report_level=short --color_output)
