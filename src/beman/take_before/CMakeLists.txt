# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

add_library(beman.take_before)
add_library(beman::take_before ALIAS beman.take_before)

target_sources(beman.take_before PRIVATE take_before.cpp)

target_sources(
    beman.take_before
    PUBLIC
        FILE_SET HEADERS
            BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/../../../include"
            FILES
                "${CMAKE_CURRENT_SOURCE_DIR}/../../../include/beman/take_before/take_before.hpp"
)

set_target_properties(beman.take_before PROPERTIES VERIFY_INTERFACE_HEADER_SETS ON)

find_package(beman-install-library REQUIRED)
beman_install_library(beman.take_before)
