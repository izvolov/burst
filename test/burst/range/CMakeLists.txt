target_sources(burst-unit-tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/bitap.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/buffered_chunks.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/cache_one.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/difference.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/intersect.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/join.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/merge.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/own_as_range.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/semiintersect.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/skip_to_lower_bound.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/skip_to_upper_bound.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/subsequences.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/subsets.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/symmetric_difference.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/take_n.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/unite.cpp
)

add_subdirectory(adaptor)
