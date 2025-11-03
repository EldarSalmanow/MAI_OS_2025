project(lab2
        LANGUAGES C)

set(CLI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cli)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)

add_executable(lab2
        cli/lab2.c
        src/lab2/geometry.c
        src/lab2/metrics.c
)

#target_compile_options(lab2
#        PRIVATE
#        -fsanitize=thread
#)

target_include_directories(lab2
        PRIVATE
        ${INCLUDE_DIR}
)

target_link_libraries(lab2
        PRIVATE
        m pthread
)

