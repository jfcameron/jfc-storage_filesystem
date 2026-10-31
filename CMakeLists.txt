# © Joseph Cameron - MIT License

cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

if (TARGET jfcstorage_filesystem)
    return()
endif()

project(jfc-storage_filesystem
    VERSION 1.0.0
    DESCRIPTION "the jfc-storage store interface, implemented over the host's filesystem"
    LANGUAGES CXX)

include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/jfc-cmake/jfclib.cmake")

include(GNUInstallDirs)

option(JFCSTORAGE_FILESYSTEM_BUILD_TESTS "Build unit tests" ${PROJECT_IS_TOP_LEVEL})
option(JFCSTORAGE_FILESYSTEM_BUILD_DOCS "Build documentation" OFF)
option(JFCSTORAGE_FILESYSTEM_INSTALL "Generate install rules" ${PROJECT_IS_TOP_LEVEL})

if (JFCSTORAGE_FILESYSTEM_BUILD_DOCS)
    jfc_generate_documentation_doxygen(
        README_PATH      README.md
        OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/docs

        INCLUDE_DIRECTORIES
            include/
    )

    if (NOT JFCSTORAGE_FILESYSTEM_BUILD_TESTS AND NOT JFC_BUILD_FUZZERS)
        return()
    endif()
endif()

add_subdirectory(extern)

if (ANDROID)
    set(_jfcstorage_user_directories "")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux" OR CMAKE_SYSTEM_NAME MATCHES "BSD$")
    set(_jfcstorage_user_directories "xdg")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(_jfcstorage_user_directories "macos")
elseif (CMAKE_SYSTEM_NAME STREQUAL "iOS")
    set(_jfcstorage_user_directories "ios")
elseif (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(_jfcstorage_user_directories "windows")
elseif (EMSCRIPTEN)
    set(_jfcstorage_user_directories "emscripten")
else()
    set(_jfcstorage_user_directories "")
endif()

set(_jfcstorage_strict_flags)

if (MSVC)
    list(APPEND _jfcstorage_strict_flags $<$<COMPILE_LANGUAGE:CXX>:/permissive->)
else()
    if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        list(APPEND _jfcstorage_strict_flags $<$<COMPILE_LANGUAGE:CXX>:-pedantic>)
    endif()

    include(CheckCXXCompilerFlag)

    foreach (_strict_flag "-Werror=c++20-extensions" "-Werror=c++23-extensions")
        string(MAKE_C_IDENTIFIER "HAS${_strict_flag}" _strict_flag_symbol)

        check_cxx_compiler_flag("${_strict_flag}" ${_strict_flag_symbol})

        if (${_strict_flag_symbol})
            list(APPEND _jfcstorage_strict_flags $<$<COMPILE_LANGUAGE:CXX>:${_strict_flag}>)
        endif()
    endforeach()
endif()

function(_jfcstorage_filesystem_library aTarget aExportName)
    add_library(${aTarget} STATIC ${ARGN})

    add_library(jfc::${aExportName} ALIAS ${aTarget})

    set_target_properties(${aTarget} PROPERTIES
        EXPORT_NAME ${aExportName}
        PREFIX "lib"
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CXX_EXTENSIONS OFF)

    target_compile_features(${aTarget} PUBLIC cxx_std_20)

    target_compile_options(${aTarget} PRIVATE ${_jfcstorage_strict_flags})

    target_include_directories(${aTarget}
        PUBLIC
            "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
            "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
        PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/src/include")

    target_link_libraries(${aTarget} PUBLIC jfcstorage_interface)
endfunction()

if (WIN32)
    set(_jfcstorage_flush "windows")
else()
    set(_jfcstorage_flush "posix")
endif()

_jfcstorage_filesystem_library(jfcstorage_filesystem storage_filesystem
    "${CMAKE_CURRENT_SOURCE_DIR}/src/filesystem_store.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/${_jfcstorage_flush}_flush.cpp")

set(_jfcstorage_filesystem_targets jfcstorage_filesystem)

if (_jfcstorage_user_directories)
    _jfcstorage_filesystem_library(jfcstorage_user_directories storage_user_directories
        "${CMAKE_CURRENT_SOURCE_DIR}/src/user_directories.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/${_jfcstorage_user_directories}_user_directories.cpp")

    if (_jfcstorage_user_directories STREQUAL "windows")
        target_link_libraries(jfcstorage_user_directories PRIVATE shell32 ole32)
    endif()

    list(APPEND _jfcstorage_filesystem_targets jfcstorage_user_directories)
endif()

if (JFCSTORAGE_FILESYSTEM_INSTALL)
    include(CMakePackageConfigHelpers)

    install(TARGETS ${_jfcstorage_filesystem_targets}
        EXPORT jfc-storage_filesystem-targets
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")

    set(_jfcstorage_filesystem_headers "${CMAKE_CURRENT_SOURCE_DIR}/include/jfc/storage/filesystem_store.h")

    if (_jfcstorage_user_directories)
        list(APPEND _jfcstorage_filesystem_headers "${CMAKE_CURRENT_SOURCE_DIR}/include/jfc/storage/user_directories.h")
    endif()

    install(FILES ${_jfcstorage_filesystem_headers} DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/jfc/storage")

    install(EXPORT jfc-storage_filesystem-targets
        FILE jfc-storage_filesystem-targets.cmake
        NAMESPACE jfc::
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/jfc-storage_filesystem")

    write_basic_package_version_file(
        "${PROJECT_BINARY_DIR}/jfc-storage_filesystem-config-version.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion)

    file(WRITE "${PROJECT_BINARY_DIR}/jfc-storage_filesystem-config.cmake"
"include(CMakeFindDependencyMacro)
find_dependency(jfcstorage 2)
include(\"\${CMAKE_CURRENT_LIST_DIR}/jfc-storage_filesystem-targets.cmake\")
")

    install(FILES
        "${PROJECT_BINARY_DIR}/jfc-storage_filesystem-config.cmake"
        "${PROJECT_BINARY_DIR}/jfc-storage_filesystem-config-version.cmake"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/jfc-storage_filesystem")
endif()

if (JFCSTORAGE_FILESYSTEM_BUILD_TESTS)
    add_subdirectory(test)
endif()

if (JFC_BUILD_FUZZERS)
    add_subdirectory(fuzz)
endif()
