add_executable(merge merge_iterator.cpp)
target_link_libraries(merge PRIVATE Burst::burst benchIO Boost::boost)

add_executable(intersect intersect_iterator.cpp)
target_link_libraries(intersect PRIVATE Burst::burst benchIO Boost::boost)

add_executable(subset subset_iterator.cpp)
target_link_libraries(subset PRIVATE Burst::burst benchIO Boost::boost)

add_executable(varit variant_iterator.cpp)
target_link_libraries(varit PRIVATE Burst::burst benchIO Boost::program_options)
