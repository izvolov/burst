set(K_ARY_SEARCH_SOURCES k_ary_search_set.cpp)
set(K_ARY_SEARCH_EXECUTABLE kary)
add_executable(${K_ARY_SEARCH_EXECUTABLE} ${K_ARY_SEARCH_SOURCES})
target_link_libraries(${K_ARY_SEARCH_EXECUTABLE} ${Boost_LIBRARIES})
