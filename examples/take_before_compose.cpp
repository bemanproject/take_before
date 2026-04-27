// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
// examples/take_before_compose.cpp

#include <beman/take_before/take_before.hpp>

#include <iostream>
#include <ranges>
#include <vector>

namespace btb = beman::take_before;

int main() {

    std::vector<int> numbers = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

    // Compose: filter even numbers, double them, take before value 12
    auto result = numbers | std::views::filter([](int x) { return x % 2 == 0; }) // {2, 4, 6, 8, 10}
                  | std::views::transform([](int x) { return x * 2; })           // {4, 8, 12, 16, 20}
                  | btb::views::take_before(12);                                 // {4, 8}

    std::cout << "Composed view result: ";
    for (int val : result) {
        std::cout << val << ' ';
    }

    std::cout << '\n'; // Output: 4 8

    return 0;
}
