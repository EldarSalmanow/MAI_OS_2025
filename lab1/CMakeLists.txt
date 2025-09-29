project(lab1
        LANGUAGES C)

set(CLI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cli)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)

#add_lab(1
#        CLI_SOURCES child.c parent.c
#        LIB_SOURCES child.c parent.c
#)

#add_cli_app(
#        LAB_NUM 1
#        CLI_NAME parent
#        CLI_SOURCES parent.c
#        LIB_SOURCES parent.c
#)
#
#add_cli_app(
#        LAB_NUM 1
#        CLI_NAME child
#        CLI_SOURCES child.c
#        LIB_SOURCES child.c
#)

add_executable(parent
        cli/parent.c
)

target_include_directories(parent
        PRIVATE
        ${INCLUDE_DIR}
)

add_executable(child
        cli/child.c
)

target_include_directories(child
        PRIVATE
        ${INCLUDE_DIR}
)
