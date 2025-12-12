project(lab4
        LANGUAGES C)

set(CLI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cli)
set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(TESTS_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)

add_library(mathematics_impl1
        SHARED
        src/lab4/mathematics_impl1.c
)

target_include_directories(mathematics_impl1
        PRIVATE
        ${INCLUDE_DIR}
)

add_library(mathematics_impl2
        SHARED
        src/lab4/mathematics_impl2.c
)

target_compile_options(mathematics_impl1
        PRIVATE
        -fPIC
)

target_include_directories(mathematics_impl2
        PRIVATE
        ${INCLUDE_DIR}
)

target_compile_options(mathematics_impl2
        PRIVATE
        -fPIC
)

add_executable(lab4_1
        cli/lab4_1.c
        src/lab4/commands.c
)

target_include_directories(lab4_1
        PRIVATE
        ${INCLUDE_DIR}
)

target_link_libraries(lab4_1
        PRIVATE
        mathematics_impl1
)

add_executable(lab4_2
        cli/lab4_2.c
        src/lab4/commands.c
)

target_include_directories(lab4_2
        PRIVATE
        ${INCLUDE_DIR}
)

target_link_libraries(lab4_2
        PRIVATE
        dl
)
