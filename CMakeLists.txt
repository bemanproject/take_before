# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

cmake_minimum_required(VERSION 3.28...4.2)

project(
    beman.take_before
    DESCRIPTION "take_before_view"
    LANGUAGES CXX
    VERSION 0.0.1
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


add_library(beman.take_before INTERFACE)
target_sources(
           beman.take_before
               INTERFACE
                 FILE_SET HEADERS
                 BASE_DIRS include
                 FILES include/beman/take_before/take_before.hpp
                       include/beman/take_before/beman.take_before-config.cmake.in
    )

add_library(beman::take_before ALIAS beman.take_before)

set(CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/infra/cmake)


find_package(beman-install-library REQUIRED)
beman_install_library(beman.take_before)

if(BEMAN_TAKE_BEFORE_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests/beman/take_before)
endif()

if(BEMAN_TAKE_BEFORE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
