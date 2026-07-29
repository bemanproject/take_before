# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

cmake_minimum_required(VERSION 3.30...4.4)

include(infra/cmake/enable-experimental-import-std.cmake)

project(
    beman.take_before
    DESCRIPTION "take_before_view"
    LANGUAGES CXX
    VERSION 0.1.0
)

# [CMAKE.SKIP_TESTS]
option(
    BEMAN_TAKE_BEFORE_BUILD_TESTS
    "Enable building tests and test infrastructure. Default: ${PROJECT_IS_TOP_LEVEL}. Values: { ON, OFF }."
    ${PROJECT_IS_TOP_LEVEL}
)

# [CMAKE.SKIP_EXAMPLES]
option(
    BEMAN_TAKE_BEFORE_BUILD_EXAMPLES
    "Enable building examples. Default: ${PROJECT_IS_TOP_LEVEL}. Values: { ON, OFF }."
    ${PROJECT_IS_TOP_LEVEL}
)

# for find of beman_install_library and configure_build_telemetry
option(
    BEMAN_TAKE_BEFORE_USE_MODULES
    "Provide beman.take_before as a C++ module"
    OFF
)

if(BEMAN_TAKE_BEFORE_USE_MODULES)
    set(CMAKE_CXX_SCAN_FOR_MODULES ON)
endif()

configure_file(
    "${PROJECT_SOURCE_DIR}/include/beman/take_before/config_generated.hpp.in"
    "${PROJECT_BINARY_DIR}/include/beman/take_before/config_generated.hpp"
    @ONLY
)

include(infra/cmake/beman-install-library.cmake)
include(infra/cmake/BuildTelemetryConfig.cmake)

if(BEMAN_TAKE_BEFORE_USE_MODULES)
    add_library(beman.take_before STATIC)
else()
    add_library(beman.take_before INTERFACE)
endif()
add_library(beman::take_before ALIAS beman.take_before)

if(BEMAN_TAKE_BEFORE_USE_MODULES)
    target_sources(
        beman.take_before
        PUBLIC
            FILE_SET CXX_MODULES
            FILE_SET HEADERS
                BASE_DIRS
                    "${CMAKE_CURRENT_SOURCE_DIR}/include"
                    "${CMAKE_CURRENT_BINARY_DIR}/include"
    )
    set_target_properties(beman.take_before PROPERTIES CXX_MODULE_STD ON)
    target_compile_features(beman.take_before PUBLIC cxx_std_23)
else()
    target_sources(
        beman.take_before
        PUBLIC
            FILE_SET HEADERS
                BASE_DIRS
                    "${CMAKE_CURRENT_SOURCE_DIR}/include"
                    "${CMAKE_CURRENT_BINARY_DIR}/include"
    )
    set_target_properties(
        beman.take_before
        PROPERTIES VERIFY_INTERFACE_HEADER_SETS ${PROJECT_IS_TOP_LEVEL}
    )
endif()

add_subdirectory(include/beman/take_before)

beman_install_library(beman.take_before TARGETS beman.take_before)
configure_build_telemetry()

if(BEMAN_TAKE_BEFORE_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests/beman/take_before)
endif()

if(BEMAN_TAKE_BEFORE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
