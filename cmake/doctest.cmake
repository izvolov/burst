set(BURST_DOCTEST_VERSION 2.4.11)
set(BURST_DOCTEST_REPOSITORY https://github.com/doctest/doctest.git)

find_package(doctest ${BURST_DOCTEST_VERSION} QUIET)

if (doctest_FOUND)
    message(STATUS "Найден doctest ${doctest_VERSION}: ${doctest_DIR}")
else()
    message(STATUS
        "doctest ${BURST_DOCTEST_VERSION} будет взят с гитхаба: ${BURST_DOCTEST_REPOSITORY}")

    find_package(Git REQUIRED)

    include(FetchContent)
    FetchContent_Declare(doctest
        GIT_REPOSITORY
            ${BURST_DOCTEST_REPOSITORY}
        GIT_TAG
            v${BURST_DOCTEST_VERSION}
        PATCH_COMMAND
            # При каждом перепрогоне cmake будет пытаться заново применить патч,
            # поэтому, если патч применить не удалось, считаем, что он уже применён.
            ${GIT_EXECUTABLE} apply ${CMAKE_SOURCE_DIR}/cmake/doctest.patch || true
    )
    FetchContent_MakeAvailable(doctest)
endif()
