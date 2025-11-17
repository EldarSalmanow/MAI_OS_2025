project(lab3
        LANGUAGES C)

set(CLI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cli)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)

add_executable(lab3_parent
        cli/parent.c
)

target_include_directories(lab3_parent
        PRIVATE
        ${INCLUDE_DIR}
)

set_target_properties(lab3_parent PROPERTIES OUTPUT_NAME "parent")

add_executable(lab3_child
        cli/child.c
)

target_include_directories(lab3_child
        PRIVATE
        ${INCLUDE_DIR}
)

set_target_properties(lab3_child PROPERTIES OUTPUT_NAME "child")
