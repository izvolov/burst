set(RANGE_GENERATOR_SOURCES range_generator.cpp)
set(RANGE_GENERATOR_EXECUTABLE rangegen)
add_executable(${RANGE_GENERATOR_EXECUTABLE} ${RANGE_GENERATOR_SOURCES})
target_link_libraries(${RANGE_GENERATOR_EXECUTABLE} ${Boost_LIBRARIES})

set(K_ARY_SEARCH_SOURCES k_ary_search_set.cpp)
set(K_ARY_SEARCH_EXECUTABLE kary)
add_executable(${K_ARY_SEARCH_EXECUTABLE} ${K_ARY_SEARCH_SOURCES})
target_link_libraries(${K_ARY_SEARCH_EXECUTABLE} ${Boost_LIBRARIES})

set(MERGE_SOURCES iterator/merge_iterator.cpp)
set(MERGE_EXECUTABLE merge)
add_executable(${MERGE_EXECUTABLE} ${MERGE_SOURCES})

set(GALLOPING_LOWER_BOUND_SOURCES algorithm/galloping_lower_bound.cpp)
set(GALLOPING_LOWER_BOUND_EXECUTABLE gallopinglb)
add_executable(${GALLOPING_LOWER_BOUND_EXECUTABLE} ${GALLOPING_LOWER_BOUND_SOURCES})
target_link_libraries(${GALLOPING_LOWER_BOUND_EXECUTABLE} ${Boost_LIBRARIES})
