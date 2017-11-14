set(DIVCEIL_SOURCES divceil.cpp)
set(DIVCEIL_EXECUTABLE divceil)
add_executable(${DIVCEIL_EXECUTABLE} ${DIVCEIL_SOURCES})
target_link_libraries(${DIVCEIL_EXECUTABLE} ${Boost_LIBRARIES})
