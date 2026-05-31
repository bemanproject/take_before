// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// examples/take_before_ntbs.cpp

#include <beman/take_before/config.hpp>

#if BEMAN_TAKE_BEFORE_USE_MODULES()

import std;

#else

    #include <iostream>

#endif

#include <beman/take_before/take_before.hpp>

namespace btb = beman::take_before;

int main() {
    const char* one_two = "One?Two";

    for (auto c : btb::views::take_before(one_two, '?')) {
        std::cout << c; // prints: One
    }
    std::cout << "\n";
}
