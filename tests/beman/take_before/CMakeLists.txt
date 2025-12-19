# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

find_package(GTest REQUIRED)

add_executable(beman.take_before.tests.take_before)
target_sources(beman.take_before.tests.take_before PRIVATE take_before.test.cpp)
target_link_libraries(
    beman.take_before.tests.take_before
    PRIVATE beman::take_before GTest::gtest GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(beman.take_before.tests.take_before)
