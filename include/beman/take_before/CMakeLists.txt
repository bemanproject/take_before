# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

if(BEMAN_TAKE_BEFORE_USE_MODULES)
    target_sources(
        beman.take_before
        PUBLIC
            FILE_SET CXX_MODULES FILES take_before.cppm
            FILE_SET HEADERS
                FILES
                    config.hpp
                    take_before.hpp
                    "${PROJECT_BINARY_DIR}/include/beman/take_before/config_generated.hpp"
    )
else()
    target_sources(
        beman.take_before
        PUBLIC
            FILE_SET HEADERS
                FILES
                    config.hpp
                    take_before.hpp
                    "${PROJECT_BINARY_DIR}/include/beman/take_before/config_generated.hpp"
    )
endif()
