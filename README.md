# beman.take_before: implementation of the `views::take_before` range adaptor

<!--
SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
-->

<!-- markdownlint-disable-next-line line-length -->
![Library Status](https://raw.githubusercontent.com/bemanproject/beman/refs/heads/main/images/badges/beman_badge-beman_library_under_development.svg) ![Continuous Integration Tests](https://github.com/bemanproject/take_before/actions/workflows/ci_tests.yml/badge.svg) ![Lint Check (pre-commit)](https://github.com/bemanproject/take_before/actions/workflows/pre-commit-check.yml/badge.svg) [![Coverage](https://coveralls.io/repos/github/bemanproject/take_before/badge.svg?branch=main)](https://coveralls.io/github/bemanproject/take_before?branch=main) ![Standard Target](https://github.com/bemanproject/beman/blob/main/images/badges/cpp29.svg)

`beman.take_before` is a C++ library implementing the `views::take_before` range adaptor as proposed in
[P3220R2](https://wg21.link/P3220R2), conforming to
[The Beman Standard](https://github.com/bemanproject/beman/blob/main/docs/beman_standard.md).

**Implements**: [`views::take_before` range adaptor (P3220R2)](https://wg21.link/P3220R2)

**Status**: [Under development and not yet ready for production use.](https://github.com/bemanproject/beman/blob/main/docs/beman_library_maturity_model.md#under-development-and-not-yet-ready-for-production-use)

## Overview

`views::take_before` accepts a range (or iterator) and a specified value and produces a range ending with the
first occurrence of that value. It's particularly useful for constructing null-terminated byte string (NTBS)
ranges without calculating their actual length.

## License

`beman.take_before` is licensed under the Apache License v2.0 with LLVM Exceptions.

## Usage

Full runable examples can be found in `examples/` - please check [./examples/README.md](./examples/README.md).

### Basic Usage: Pipe Syntax

-  [![Basic Usage@Compiler Explorer](https://img.shields.io/badge/Try%20it%20on%20Compiler%20Explorer-grey?logo=compilerexplorer&logoColor=67c52a)](https://godbolt.org/z/hf671dfof)


```cpp
#include <beman/take_before/take_before.hpp>
#include <vector>
#include <iostream>

namespace btb = beman::take_before;

int main() {
    std::vector<int> v = {1, 2, 3, 4, 5};

    // Take elements before the first occurrence of 3
    for (int i : v | btb::views::take_before(3)) {
        std::cout << i << ' ';  // prints: 1 2
    }
}
```

### NTBS (Null-Terminated Byte String) - Primary Use Case

-  [![NTBS @Compiler Explorer](https://img.shields.io/badge/Try%20it%20on%20Compiler%20Explorer-grey?logo=compilerexplorer&logoColor=67c52a)](https://godbolt.org/z/9ad7qE7v9)

```cpp
#include <beman/take_before/take_before.hpp>
#include <iostream>

namespace btb = beman::take_before;

int main() {
    const char* one_two = "One?Two";

    for (auto c : btb::views::take_before(one_two, '?')) {
        std::cout << c;  // prints: One
    }
}
```

### Composition with Other Views

-  [![Composition @Compiler Explorer](https://img.shields.io/badge/Try%20it%20on%20Compiler%20Explorer-grey?logo=compilerexplorer&logoColor=67c52a)](https://godbolt.org/z/GdEsxzKdf)

```cpp
#include <beman/take_before/take_before.hpp>
#include <ranges>
#include <vector>
#include <print>

namespace btb = beman::take_before;

int main() {
    std::vector<int> v = {1, 2, 3, 4, 5};

    auto result = v
        | std::views::transform([](int x) { return x * 2; })
        | btb::views::take_before(6);  // {2, 4}

    //c++23
    std::println("{}", result);
}
```

## Dependencies

### Build Environment

This project requires at least the following to build:

* A C++ compiler that conforms to the C++20 standard or greater
* CMake 3.30 or later
* (Test Only) GoogleTest

You can disable building tests by setting CMake option `BEMAN_TAKE_BEFORE_BUILD_TESTS` to
`OFF` when configuring the project.

### Supported Platforms

| Compiler   | Version | C++ Standards | Standard Library  |
|------------|---------|---------------|-------------------|
| GCC        | 16-13   | C++26-C++20   | libstdc++         |
| GCC        | 12-11   | C++23, C++20  | libstdc++         |
| Clang      | 22-19   | C++26-C++20   | libstdc++, libc++ |
| Clang      | 18      | C++26-C++20   | libc++            |
| Clang      | 18      | C++23-C++20   | libstdc++         |
| Clang      | 17      | C++26-C++20   | libc++            |
| Clang      | 17      | C++20         | libstdc++         |
| AppleClang | latest  | C++26-C++20   | libc++            |
| MSVC       | latest  | C++23         | MSVC STL          |

## Development

See the [Contributing Guidelines](CONTRIBUTING.md).

## Integrate beman.take_before into your project

### Build

You can build take_before using a CMake workflow preset:

```bash
cmake --workflow --preset gcc-release
```

To list available workflow presets, you can invoke:

```bash
cmake --list-presets=workflow
```

For details on building beman.take_before without using a CMake preset, refer to the
[Contributing Guidelines](CONTRIBUTING.md).

### Installation

#### Vcpkg

The preferred way to install take_before is via vcpkg. To do so, after installing vcpkg
itself, you need to add support for the Beman project's [vcpkg
registry](https://github.com/bemanproject/vcpkg-registry) by configuring a
`vcpkg-configuration.json` file (which take_before [provides](vcpkg-configuration.json)).

Then, simply run `vcpkg install beman-take-before`.

#### Manual

To install beman.take_before globally after building with the `gcc-release` preset, you can
run:

```bash
sudo cmake --install build/gcc-release
```

Alternatively, to install to a prefix, for example `/opt/beman`, you can run:

```bash
sudo cmake --install build/gcc-release --prefix /opt/beman
```

This will generate the following directory structure:

```txt
/opt/beman
├── include
│   └── beman
│       └── take_before
│           ├── take_before.hpp
│           └── ...
└── lib
    └── cmake
        └── beman.take_before
            ├── beman.take_before-config-version.cmake
            ├── beman.take_before-config.cmake
            └── beman.take_before-targets.cmake
```

### CMake Configuration

If you installed beman.take_before to a prefix, you can specify that prefix to your CMake
project using `CMAKE_PREFIX_PATH`; for example, `-DCMAKE_PREFIX_PATH=/opt/beman`.

You need to bring in the `beman.take_before` package to define the `beman::take_before` CMake
target:

```cmake
find_package(beman.take_before REQUIRED)
```

You will then need to add `beman::take_before` to the link libraries of any libraries or
executables that include `beman.take_before` headers.

```cmake
target_link_libraries(yourlib PUBLIC beman::take_before)
```

### Using beman.take_before

To use `beman.take_before` in your C++ project,
include an appropriate `beman.take_before` header from your source code.

```c++
#include <beman/take_before/take_before.hpp>
```

> [!NOTE]
>
> `beman.take_before` headers are to be included with the `beman/take_before/` prefix.
> Altering include search paths to spell the include target another way (e.g.
> `#include <take_before.hpp>`) is unsupported.

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

When the delimiter type satisfies `tidy_obj` and the underlying range is borrowed, `take_before_view` is also
a borrowed range.

## References

- [P3220R2: views::take before](https://wg21.link/P3220R2)
- [P2760: Tier 1 Ranges Adaptors](https://wg21.link/P2760)
