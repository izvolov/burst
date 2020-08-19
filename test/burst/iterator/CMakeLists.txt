target_sources(burst-unit-tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/binary_stream_iterators.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/bitap_iterator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/buffered_chunk_iterator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cache_iterator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/difference_iterator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/intersect_iterator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/join_iterator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/merge_iterator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/owning_iterator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/semiintersect_iterator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/subsequence_iterator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/subset_iterator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/symmetric_difference_iterator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/take_at_most_iterator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/take_exactly_iterator.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/union_iterator.cpp
)
