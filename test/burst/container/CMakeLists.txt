target_sources(${UNIT_TEST_EXECUTABLE} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/dynamic_tuple.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/k_ary_search_set.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/make_sequence_container.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/make_set.cpp
)

add_subdirectory(access)
