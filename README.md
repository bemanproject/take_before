# beman::take_before

<!--
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

![Library Status](https://raw.githubusercontent.com/bemanproject/beman/refs/heads/main/images/badges/beman_badge-beman_library_under_development.svg)
[![Continuous Integration Tests](https://github.com/bemanproject/take_before/actions/workflows/ci_tests.yml/badge.svg)](https://github.com/bemanproject/take_before/actions/workflows/ci_tests.yml)
[![Lint Check (pre-commit)](https://github.com/bemanproject/take_before/actions/workflows/pre-commit-check.yml/badge.svg)](https://github.com/bemanproject/take_before/actions/workflows/pre-commit-check.yml)
![Standard Target](https://github.com/bemanproject/beman/blob/main/images/badges/cpp29.svg)

`beman.take_before` is a C++ library implementing the `views::take_before` range adaptor as proposed in [P3220R2](https://wg21.link/P3220R2), conforming to [The Beman Standard](https://github.com/bemanproject/beman/blob/main/docs/beman_standard.md).

**Implements**: `views::take_before` range adaptor.

**Status**: [Under development and not yet ready for production use.](https://github.com/bemanproject/beman/blob/main/docs/beman_library_maturity_model.md#under-development-and-not-yet-ready-for-production-use)

## Overview

`views::take_before` accepts a range (or iterator) and a specified value and produces a range ending with the first occurrence of that value. It's particularly useful for constructing null-terminated byte string (NTBS) ranges without calculating their actual length.

## License

`beman.take_before` is licensed under the Apache License v2.0 with LLVM Exceptions.

## Usage

### Basic Usage: Pipe Syntax

```cpp
#include <beman/take_before/take_before.hpp>
#include <vector>
#include <iostream>

namespace beman = beman::take_before;

int main() {
    std::vector<int> v = {1, 2, 3, 4, 5};

    // Take elements before the first occurrence of 3
    for (int i : v | beman::views::take_before(3)) {
        std::cout << i << ' ';  // prints: 1 2
    }
}
```

### NTBS (Null-Terminated Byte String) - Primary Use Case

```cpp
#include <beman/take_before/take_before.hpp>
#include <iostream>

namespace beman = beman::take_before;

int main() {
    const char* one_two = "One?Two";

    for (auto c : beman::views::take_before(one_two, '?')) {
        std::cout << c;  // prints: One
    }
}
```

### Direct Usage with Ranges

```cpp
#include <beman/take_before/take_before.hpp>
#include <vector>
#include <algorithm>

namespace beman = beman::take_before;

int main() {
    std::vector<int> v = {10, 20, 30, 40};
    auto result = beman::views::take_before(v, 30);

    // result contains: {10, 20}
}
```

### Composition with Other Views

```cpp
#include <beman/take_before/take_before.hpp>
#include <ranges>
#include <vector>

namespace beman = beman::take_before;

int main() {
    std::vector<int> v = {1, 2, 3, 4, 5};

    auto result = v
        | std::views::transform([](int x) { return x * 2; })
        | beman::views::take_before(6);  // {2, 4}
}
```

Full runnable examples can be found in [`examples/`](examples/).

## Dependencies

### Build Environment

This project requires:

* A C++ compiler that supports **C++20** or greater
* CMake 3.25 or later
* (Test Only) GoogleTest

You can disable building tests by setting CMake option `BEMAN_TAKE_BEFORE_BUILD_TESTS` to `OFF`.

### Supported Platforms

This project officially supports:

* GCC versions 11–15
* LLVM Clang++ (with libstdc++ or libc++) versions 17–21
* AppleClang version 17.0.0
* MSVC version 19.44.35215.0

## Development

### Configure and Build Using CMake Presets

```shell
cmake --workflow --preset gcc-debug
```

Available presets include `gcc-debug`, `gcc-release`, `clang-debug`, `clang-release`, etc.

### Configure and Build Manually

```bash
cmake -B build -S . -DCMAKE_CXX_STANDARD=20
cmake --build build
ctest --test-dir build
```

### Fetching GTest from GitHub

```shell
cmake -B build -S . \
    -DCMAKE_PROJECT_TOP_LEVEL_INCLUDES=./infra/cmake/use-fetch-content.cmake \
    -DCMAKE_CXX_STANDARD=20
cmake --build build --target all
ctest --test-dir build
```

## Integration

### CMake

```cmake
find_package(beman.take_before REQUIRED)
target_link_libraries(yourlib PUBLIC beman::take_before)
```

### Include Header

```cpp
#include <beman/take_before/take_before.hpp>
```

## API Reference

### `views::take_before`

```cpp
namespace beman::take_before::views {
    inline constexpr /* unspecified */ take_before;
}
```

**Parameters:**
- `range` - A viewable range or input iterator
- `value` - The delimiter value to search for

**Returns:** A view of elements from the beginning up to (but not including) the first occurrence of `value`.

**Behavior:**
- If `value` is not found, returns the entire range
- If `value` is the first element, returns an empty range
- Stops at the **first** occurrence of `value`

### `take_before_view`

```cpp
template<std::ranges::view V, std::move_constructible T>
    requires std::ranges::input_range<V> && std::is_object_v<T> &&
             std::indirect_binary_predicate<std::ranges::equal_to,
                                            std::ranges::iterator_t<V>, const T*>
class take_before_view;
```

### `tidy_obj` Concept

```cpp
template<class T>
constexpr bool tidy_obj =
    std::is_empty_v<T> &&
    std::is_trivially_default_constructible_v<T> &&
    std::is_trivially_destructible_v<T>;
```

When the delimiter type satisfies `tidy_obj` and the underlying range is borrowed, `take_before_view` is also a borrowed range.

## References

- [P3220R2: views::take_before](https://wg21.link/P3220R2)
- [P2760: Tier 1 Ranges Adaptors](https://wg21.link/P2760)
