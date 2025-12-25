// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/take_before/take_before.hpp>

#include <iostream>
#include <vector>

namespace btb = beman::take_before;

int main() {
    std::vector<int> v = {10, 20, 30, 40};
    std::cout << "Original: ";
    for (int i : v)
        std::cout << i << ' ';
    std::cout << "\nTake before 30: ";
    // Direct function call syntax (alternative to pipe syntax)
    for (int i : btb::views::take_before(v, 30)) {
        std::cout << i << ' ';
    }
    std::cout << '\n';
    return 0;
}
