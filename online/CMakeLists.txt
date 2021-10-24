find_package(Boost 1.66.0 REQUIRED)

add_executable(burst-example burst_example.cpp)
target_link_libraries(burst-example
    PRIVATE
        Boost::boost
        Burst::burst
)

add_custom_target(run-burst-example ALL COMMAND burst-example)

find_package(Python3 COMPONENTS Interpreter)
if(Python3_FOUND)
    set(WANDBOX_URL "https://wandbox.org/api/compile.json")

    add_custom_target(wandbox
        COMMAND
            Python3::Interpreter wandbox.py burst_example.cpp "${PROJECT_SOURCE_DIR}" include |
            curl -H "Content-type: application/json" -d @- ${WANDBOX_URL}
        WORKING_DIRECTORY
            ${CMAKE_CURRENT_SOURCE_DIR}
        DEPENDS
            burst-unit-tests
            run-burst-example
    )
else()
    message(WARNING "Для создания онлайн-песочницы требуется интерпретатор ЯП python 3-й версии")
endif()
