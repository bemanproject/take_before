// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// examples/take_before_text_split.cpp

// This example demonstrates the usage of beman::take_before in a range-printer.
// Requires: range support (C++20).

#include <beman/take_before/config.hpp>

#if BEMAN_TAKE_BEFORE_USE_MODULES()

import std;

#else

    #include <algorithm>
    #include <iostream>
    #include <ranges>
    #include <string>
    #include <vector>

#endif

#include <beman/take_before/take_before.hpp>

namespace btb = beman::take_before;

int main() {
    const std::string text = "Hello, world! Stop here.";

    std::cout << "Full string: " << text << '\n';

    std::cout << "Take before '!': ";
    for (char c : text | btb::views::take_before('!')) {
        std::cout << c;
    }
    std::cout << '\n';

    return 0;
}
