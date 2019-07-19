file(GLOB_RECURSE BURST_HEADERS
    RELATIVE
        ${Burst_SOURCE_DIR}
    ${Burst_SOURCE_DIR}/include/*.hpp
)
configure_file(burst-example.py.in burst-example.py @ONLY)
set(WANDBOX_URL "https://wandbox.org/api/compile.json")
add_custom_target(wandbox
    COMMAND
        python burst-example.py | curl -H "Content-type: application/json" -d @- ${WANDBOX_URL}
    DEPENDS burst-unit-tests)
