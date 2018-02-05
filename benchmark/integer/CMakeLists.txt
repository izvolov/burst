add_executable(divceil divceil.cpp)
target_link_libraries(divceil PRIVATE Burst::burst benchIO Boost::program_options)
