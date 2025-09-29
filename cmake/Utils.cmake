function(setup_project)
    include(FetchContent)

    FetchContent_Declare(
            googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG        v1.17.0
    )
    FetchContent_MakeAvailable(googletest)

    add_library(GTest::GTest INTERFACE IMPORTED)

    target_link_libraries(GTest::GTest INTERFACE gtest_main)

    add_compile_options(
            -Wall
            -Werror
            -Wextra
            -Wpedantic
    )

    enable_testing()
endfunction()

function(add_lab LAB_NUM)
    # Parsing arguments

    cmake_parse_arguments(ARG "" "" "CLI_SOURCES;LIB_SOURCES;TEST_SOURCES" ${ARGN})

    set(LAB${LAB_NUM}_CLI_SOURCES)
    foreach(SOURCE ${ARG_CLI_SOURCES})
        list(APPEND
                LAB${LAB_NUM}_CLI_SOURCES
                ${CLI_DIR}/${SOURCE}
        )
    endforeach()

    set(LAB${LAB_NUM}_LIB_SOURCES)
    foreach(SOURCE ${ARG_LIB_SOURCES})
        list(APPEND
                LAB${LAB_NUM}_LIB_SOURCES
                ${SOURCE_DIR}/lab${LAB_NUM}/${SOURCE}
        )
    endforeach()

    set(LAB${LAB_NUM}_TEST_SOURCES)
    foreach(SOURCE ${ARG_TEST_SOURCES})
        list(APPEND
                LAB${LAB_NUM}_TEST_SOURCES
                ${TESTS_DIR}/${SOURCE}
        )
    endforeach()

    # Adding library

    add_library(lab${LAB_NUM}_lib
            ${LAB${LAB_NUM}_LIB_SOURCES}
    )

    target_include_directories(lab${LAB_NUM}_lib
            PRIVATE
            ${INCLUDE_DIR}
    )

    # Adding CLI app

    if (LAB${LAB_NUM}_CLI_SOURCES)
        add_executable(lab${LAB_NUM}
                ${LAB${LAB_NUM}_CLI_SOURCES}
        )

        target_include_directories(lab${LAB_NUM}
                PRIVATE
                ${INCLUDE_DIR}
        )

        target_link_libraries(lab${LAB_NUM}
                PRIVATE
                lab${LAB_NUM}_lib
        )
    endif()

    # Adding tests

    if (LAB${LAB_NUM}_TEST_SOURCES)
        add_executable(lab${LAB_NUM}_test
                ${LAB${LAB_NUM}_TEST_SOURCES}
        )

        target_include_directories(lab${LAB_NUM}_test
                PRIVATE
                GTest::GTest
                ${INCLUDE_DIR}
        )

        target_link_libraries(lab${LAB_NUM}_test
                PRIVATE
                GTest::GTest
                lab${LAB_NUM}_lib
        )

        add_test(
                NAME lab${LAB_NUM}_tests
                COMMAND lab${LAB_NUM}_test
        )
    endif()
endfunction()