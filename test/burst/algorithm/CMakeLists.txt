target_sources(burst-unit-tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/counting_sort.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/galloping_lower_bound.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/galloping_upper_bound.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/next_subsequence.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/next_subset.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/radix_sort.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/searching/bitap.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/searching/element_position_bitmask_table.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/select_min.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/sum.cpp
)
