target_sources(burst-unit-tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/buffered_chunked.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cached_one.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/intersected.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/joined.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/merged.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/owned_as_range.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/semiintersected.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/symmetric_differenced.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/taken_at_most.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/taken_exactly.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/to_deque.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/to_forward_list.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/to_list.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/to_set.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/to_vector.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/united.cpp
)
