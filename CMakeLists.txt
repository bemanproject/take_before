# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

cmake_minimum_required(VERSION 3.25)

project(
    beman.take_before # CMake Project Name, which is also the name of the top-level
    # targets (e.g., library, executable, etc.).
    DESCRIPTION "A Beman Library for take_before"
    LANGUAGES CXX
    VERSION 2.2.1
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

include(CTest)

add_subdirectory(src/beman/take_before)

if(BEMAN_TAKE_BEFORE_BUILD_TESTS)
    add_subdirectory(tests/beman/take_before)
endif()

if(BEMAN_TAKE_BEFORE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
