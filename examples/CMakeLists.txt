# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

set(ALL_EXAMPLES take_before_direct_usage take_before_as_default_projection)
message("Examples to be built: ${ALL_EXAMPLES}")

foreach(example ${ALL_EXAMPLES})
    add_executable(beman.take_before.examples.${example})
    target_sources(beman.take_before.examples.${example} PRIVATE ${example}.cpp)
    target_link_libraries(
        beman.take_before.examples.${example}
        PRIVATE beman::take_before
    )
endforeach()
