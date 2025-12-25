# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

# gersemi: off

set(ALL_EXAMPLES take_before_direct_usage)

message("Examples to be built: ${ALL_EXAMPLES}")

foreach(example ${ALL_EXAMPLES})
    add_executable(${example})
    target_sources(${example} PRIVATE ${example}.cpp)
    target_link_libraries(
        ${example}
        PRIVATE beman::take_before
    )
endforeach()
