# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

find_package(GTest QUIET)

add_executable(beman_take_before.test)
target_sources(beman_take_before.test PRIVATE take_before.test.cpp)
target_link_libraries(
    beman_take_before.test
    PRIVATE beman::take_before GTest::gtest GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(beman_take_before.test)
