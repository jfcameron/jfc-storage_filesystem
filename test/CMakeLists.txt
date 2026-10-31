# © Joseph Cameron - MIT License

set(_jfcstorage_test_sources "${CMAKE_CURRENT_LIST_DIR}/filesystem_store_test.cpp")

set(_jfcstorage_test_libraries jfcstorage_filesystem jfcstorage_ext jfcstorage_conformance)

if (_jfcstorage_user_directories)
    list(APPEND _jfcstorage_test_sources "${CMAKE_CURRENT_LIST_DIR}/user_directories_test.cpp")

    list(APPEND _jfcstorage_test_libraries jfcstorage_user_directories)

    if (EXISTS "${CMAKE_CURRENT_LIST_DIR}/${_jfcstorage_user_directories}_user_directories_test.cpp")
        list(APPEND _jfcstorage_test_sources
            "${CMAKE_CURRENT_LIST_DIR}/${_jfcstorage_user_directories}_user_directories_test.cpp")
    endif()
endif()

if (NOT EMSCRIPTEN)
    find_package(Threads REQUIRED)

    list(APPEND _jfcstorage_test_libraries Threads::Threads)
endif()

jfc_add_tests(
    C++_STANDARD 20

    TEST_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/filesystem_conformance.cpp"
        ${_jfcstorage_test_sources}

    LIBRARIES
        ${_jfcstorage_test_libraries}
)

set_property(TARGET ${JFC_TEST_TARGET} PROPERTY CXX_EXTENSIONS OFF)

jfc_add_tests(
    C++_STANDARD 20

    TEST_SOURCE_FILES
        "${CMAKE_CURRENT_LIST_DIR}/flushed_filesystem_conformance.cpp"

    LIBRARIES
        jfcstorage_filesystem
        jfcstorage_conformance
)

set_property(TARGET ${JFC_TEST_TARGET} PROPERTY CXX_EXTENSIONS OFF)

if (NOT CMAKE_CROSSCOMPILING AND JFCSTORAGE_FILESYSTEM_INSTALL)
    add_test(NAME jfc-storage_filesystem_install_and_consume
        COMMAND "${CMAKE_COMMAND}"
            "-DBUILD_DIR=${PROJECT_BINARY_DIR}"
            "-DWORK_DIR=${CMAKE_CURRENT_BINARY_DIR}/install_and_consume"
            "-DCONFIG=$<CONFIG>"
            "-DGENERATOR=${CMAKE_GENERATOR}"
            "-DCXX_COMPILER=${CMAKE_CXX_COMPILER}"
            "-DCXX_FLAGS=${CMAKE_CXX_FLAGS}"
            "-DEXE_LINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}"
            -P "${CMAKE_CURRENT_LIST_DIR}/install/install_and_consume.cmake")
endif()
