add_executable(rangegen range_generator.cpp)
target_link_libraries(rangegen PRIVATE Burst::burst benchIO Boost::program_options)

add_executable(display display.cpp)
target_link_libraries(display PRIVATE Burst::burst benchIO Boost::program_options)
