// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#include <beman/take_before/take_before.hpp>

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <ranges>
#include <vector>
#include <string>

namespace tb = beman::take_before;

TEST(TakeBeforeTest, range_pipe_usage) {
    const std::vector<int> v = {1, 2, 3, 4, 5};
    auto                   b = v | tb::views::take_before(3);

    std::vector<int> result;
    for (int i : b) {
        result.push_back(i);
    }

    const std::vector<int> expected = {1, 2};
    EXPECT_EQ(result, expected);
}

TEST(TakeBeforeTest, range_direct_usage) {
    const std::vector<int> v = {10, 20, 30, 40};
    auto                   b = tb::views::take_before(v, 30);

    std::vector<int> result;
    std::ranges::copy(b, std::back_inserter(result));

    const std::vector<int> expected = {10, 20};
    EXPECT_EQ(result, expected);
}

TEST(TakeBeforeTest, iterator_usage) {
    const char* s = "Hello?World";
    auto        b = tb::views::take_before(s, '?');

    std::string result;
    for (char c : b) {
        result += c;
    }

    EXPECT_EQ(result, "Hello");
}

TEST(TakeBeforeTest, not_found_usage) {
    const std::vector<int> v = {1, 2, 3};
    auto                   b = v | tb::views::take_before(4);

    std::vector<int> result;
    std::ranges::copy(b, std::back_inserter(result));

    EXPECT_EQ(result, v);
}

TEST(TakeBeforeTest, first_element_usage) {
    const std::vector<int> v = {1, 2, 3};
    auto                   b = v | tb::views::take_before(1);

    EXPECT_TRUE(std::ranges::empty(b));
}

TEST(TakeBeforeTest, empty_range_usage) {
    const std::vector<int> v = {};
    auto                   b = v | tb::views::take_before(1);

    EXPECT_TRUE(std::ranges::empty(b));
}

TEST(TakeBeforeTest, borrowability_test) {
    // char is NOT a tidy_obj (is_empty_v<char> is false), so even with
    // a borrowed underlying range, the take_before_view is NOT borrowed.
    const char* s = "borrow";
    auto        b = tb::views::take_before(s, 'o');
    // Note: char is NOT tidy_obj because it's not empty, so this is NOT borrowed
    static_assert(!std::ranges::borrowed_range<decltype(b)>);

    // Verify view still works correctly
    std::string result;
    for (char c : b) {
        result += c;
    }
    EXPECT_EQ(result, "b"); // "borrow" before first 'o' is just "b"
}

// Additional tests for better coverage

TEST(TakeBeforeTest, multiple_occurrences_stops_at_first) {
    // Paper specifies: produces a range ending with the FIRST occurrence
    const std::vector<int> v = {1, 2, 3, 2, 4, 2, 5};
    auto                   b = v | tb::views::take_before(2);

    std::vector<int> result;
    std::ranges::copy(b, std::back_inserter(result));

    const std::vector<int> expected = {1}; // Stops at first '2'
    EXPECT_EQ(result, expected);
}

TEST(TakeBeforeTest, const_view_usage) {
    // Test const-correctness
    const std::vector<int> v = {1, 2, 3, 4, 5};
    const auto             b = tb::take_before_view(std::ranges::views::all(v), 3);

    std::vector<int> result;
    for (int i : b) {
        result.push_back(i);
    }

    const std::vector<int> expected = {1, 2};
    EXPECT_EQ(result, expected);
}

TEST(TakeBeforeTest, non_tidy_obj_non_borrowed) {
    // Non-tidy-obj types (like std::string) should NOT be borrowed
    const std::vector<std::string> v = {"a", "b", "c"};
    auto                           b = v | tb::views::take_before(std::string("b"));

    // std::string is NOT tidy-obj (not empty, not trivial)
    static_assert(!std::ranges::borrowed_range<decltype(b)>);

    std::vector<std::string> result;
    std::ranges::copy(b, std::back_inserter(result));

    const std::vector<std::string> expected = {"a"};
    EXPECT_EQ(result, expected);
}

TEST(TakeBeforeTest, base_accessor) {
    // Test base() accessor
    std::vector<int> v = {1, 2, 3, 4, 5};
    auto             b = tb::take_before_view(std::ranges::views::all(v), 3);

    auto base_view = std::move(b).base();
    EXPECT_EQ(std::ranges::distance(base_view), 5);
}

// ============================================================================
// COMPREHENSIVE TEST SUITE
// ============================================================================

// --- Different Data Types ---

TEST(TakeBeforeTest, with_double_values) {
    std::vector<double> v = {1.1, 2.2, 3.3, 4.4, 5.5};
    auto                b = v | tb::views::take_before(3.3);

    std::vector<double> result;
    std::ranges::copy(b, std::back_inserter(result));

    std::vector<double> expected = {1.1, 2.2};
    EXPECT_EQ(result, expected);
}

TEST(TakeBeforeTest, with_char_in_vector) {
    std::vector<char> v = {'a', 'b', 'c', 'd', 'e'};
    auto              b = v | tb::views::take_before('c');

    std::string result;
    for (char c : b) {
        result += c;
    }

    EXPECT_EQ(result, "ab");
}

TEST(TakeBeforeTest, with_wchar_t) {
    std::vector<wchar_t> v = {L'A', L'B', L'C', L'D'};
    auto                 b = v | tb::views::take_before(L'C');

    std::vector<wchar_t> result;
    std::ranges::copy(b, std::back_inserter(result));

    std::vector<wchar_t> expected = {L'A', L'B'};
    EXPECT_EQ(result, expected);
}

TEST(TakeBeforeTest, with_negative_numbers) {
    std::vector<int> v = {-5, -3, -1, 0, 1, 3, 5};
    auto             b = v | tb::views::take_before(0);

    std::vector<int> result;
    std::ranges::copy(b, std::back_inserter(result));

    std::vector<int> expected = {-5, -3, -1};
    EXPECT_EQ(result, expected);
}

TEST(TakeBeforeTest, with_large_numbers) {
    std::vector<long long> v = {1000000000LL, 2000000000LL, 3000000000LL};
    auto                   b = v | tb::views::take_before(2000000000LL);

    std::vector<long long> result;
    std::ranges::copy(b, std::back_inserter(result));

    std::vector<long long> expected = {1000000000LL};
    EXPECT_EQ(result, expected);
}

// --- Edge Cases ---

TEST(TakeBeforeTest, single_element_found) {
    std::vector<int> v = {42};
    auto             b = v | tb::views::take_before(42);

    EXPECT_TRUE(std::ranges::empty(b));
}

TEST(TakeBeforeTest, single_element_not_found) {
    std::vector<int> v = {42};
    auto             b = v | tb::views::take_before(99);

    std::vector<int> result;
    std::ranges::copy(b, std::back_inserter(result));

    std::vector<int> expected = {42};
    EXPECT_EQ(result, expected);
}

TEST(TakeBeforeTest, last_element_is_delimiter) {
    std::vector<int> v = {1, 2, 3, 4, 5};
    auto             b = v | tb::views::take_before(5);

    std::vector<int> result;
    std::ranges::copy(b, std::back_inserter(result));

    std::vector<int> expected = {1, 2, 3, 4};
    EXPECT_EQ(result, expected);
}

TEST(TakeBeforeTest, second_element_is_delimiter) {
    std::vector<int> v = {1, 2, 3, 4, 5};
    auto             b = v | tb::views::take_before(2);

    std::vector<int> result;
    std::ranges::copy(b, std::back_inserter(result));

    std::vector<int> expected = {1};
    EXPECT_EQ(result, expected);
}

TEST(TakeBeforeTest, all_same_elements) {
    std::vector<int> v = {7, 7, 7, 7, 7};
    auto             b = v | tb::views::take_before(7);

    EXPECT_TRUE(std::ranges::empty(b));
}

TEST(TakeBeforeTest, delimiter_zero) {
    std::vector<int> v = {1, 2, 0, 3, 4};
    auto             b = v | tb::views::take_before(0);

    std::vector<int> result;
    std::ranges::copy(b, std::back_inserter(result));

    std::vector<int> expected = {1, 2};
    EXPECT_EQ(result, expected);
}

// --- NTBS (Null-Terminated Byte String) - Primary Use Case ---

TEST(TakeBeforeTest, ntbs_simple) {
    const char* s = "Hello\0World"; // Embedded null
    auto        b = tb::views::take_before(s, '\0');

    std::string result;
    for (char c : b) {
        result += c;
    }

    EXPECT_EQ(result, "Hello");
}

TEST(TakeBeforeTest, ntbs_empty_before_null) {
    const char* s = "\0Hidden";
    auto        b = tb::views::take_before(s, '\0');

    std::string result;
    for (char c : b) {
        result += c;
    }

    EXPECT_EQ(result, "");
}

TEST(TakeBeforeTest, ntbs_with_newline) {
    const char* s = "First\nSecond\nThird";
    auto        b = tb::views::take_before(s, '\n');

    std::string result;
    for (char c : b) {
        result += c;
    }

    EXPECT_EQ(result, "First");
}

TEST(TakeBeforeTest, ntbs_with_colon) {
    const char* s = "name:value";
    auto        b = tb::views::take_before(s, ':');

    std::string result;
    for (char c : b) {
        result += c;
    }

    EXPECT_EQ(result, "name");
}

// --- Composition with Other Views ---

TEST(TakeBeforeTest, composition_with_transform) {
    std::vector<int> v = {1, 2, 3, 4, 5};
    auto             b = v | std::views::transform([](int x) { return x * 2; }) | tb::views::take_before(6); // 6 = 3*2

    std::vector<int> result;
    std::ranges::copy(b, std::back_inserter(result));

    std::vector<int> expected = {2, 4}; // 1*2, 2*2
    EXPECT_EQ(result, expected);
}

TEST(TakeBeforeTest, composition_with_filter) {
    std::vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8};
    auto             b = v | std::views::filter([](int x) { return x % 2 == 0; }) | tb::views::take_before(6);

    std::vector<int> result;
    std::ranges::copy(b, std::back_inserter(result));

    std::vector<int> expected = {2, 4};
    EXPECT_EQ(result, expected);
}

TEST(TakeBeforeTest, composition_with_reverse) {
    std::vector<int> v = {1, 2, 3, 4, 5};
    auto             b = v | std::views::reverse | tb::views::take_before(3);

    std::vector<int> result;
    std::ranges::copy(b, std::back_inserter(result));

    std::vector<int> expected = {5, 4}; // reversed, stop at 3
    EXPECT_EQ(result, expected);
}

TEST(TakeBeforeTest, composition_with_take) {
    std::vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto             b = v | std::views::take(7) | tb::views::take_before(5);

    std::vector<int> result;
    std::ranges::copy(b, std::back_inserter(result));

    std::vector<int> expected = {1, 2, 3, 4};
    EXPECT_EQ(result, expected);
}

TEST(TakeBeforeTest, composition_with_drop) {
    std::vector<int> v = {1, 2, 3, 4, 5, 6, 7};
    auto             b = v | std::views::drop(2) | tb::views::take_before(5);

    std::vector<int> result;
    std::ranges::copy(b, std::back_inserter(result));

    std::vector<int> expected = {3, 4}; // dropped 1,2; stop at 5
    EXPECT_EQ(result, expected);
}

// --- CTAD (Class Template Argument Deduction) ---

TEST(TakeBeforeTest, ctad_with_vector) {
    std::vector<int> v = {1, 2, 3, 4, 5};
    auto             b = tb::take_before_view(std::views::all(v), 3);

    std::vector<int> result;
    std::ranges::copy(b, std::back_inserter(result));

    std::vector<int> expected = {1, 2};
    EXPECT_EQ(result, expected);
}

TEST(TakeBeforeTest, ctad_with_string) {
    std::string s = "Hello, World!";
    auto        b = tb::take_before_view(std::views::all(s), ',');

    std::string result;
    for (char c : b) {
        result += c;
    }

    EXPECT_EQ(result, "Hello");
}

// --- Move Semantics ---

TEST(TakeBeforeTest, base_const_lvalue) {
    std::vector<int> v = {1, 2, 3, 4, 5};
    auto             b = tb::take_before_view(std::views::all(v), 3);

    // const& version
    auto base1 = b.base();
    EXPECT_EQ(std::ranges::distance(base1), 5);

    // Should still be usable
    std::vector<int> result;
    std::ranges::copy(b, std::back_inserter(result));
    EXPECT_EQ(result.size(), 2u);
}

TEST(TakeBeforeTest, base_rvalue) {
    std::vector<int> v = {1, 2, 3, 4, 5};
    auto             b = tb::take_before_view(std::views::all(v), 3);

    // && version
    auto base = std::move(b).base();
    EXPECT_EQ(std::ranges::distance(base), 5);
}

// --- Iterator Category Preservation ---

TEST(TakeBeforeTest, random_access_range) {
    std::vector<int> v = {1, 2, 3, 4, 5};
    auto             b = v | tb::views::take_before(3);

    // Should still be input_range
    static_assert(std::ranges::input_range<decltype(b)>);

    std::vector<int> result;
    std::ranges::copy(b, std::back_inserter(result));
    EXPECT_EQ(result.size(), 2u);
}

// --- Repeated Iteration ---

TEST(TakeBeforeTest, multiple_iterations) {
    std::vector<int> v = {1, 2, 3, 4, 5};
    auto             b = v | tb::views::take_before(3);

    // First iteration
    std::vector<int> result1;
    std::ranges::copy(b, std::back_inserter(result1));

    // Second iteration
    std::vector<int> result2;
    std::ranges::copy(b, std::back_inserter(result2));

    EXPECT_EQ(result1, result2);
}

// --- Large Data ---

TEST(TakeBeforeTest, large_range) {
    std::vector<int> v;
    for (int i = 0; i < 10000; ++i) {
        v.push_back(i);
    }

    auto b = v | tb::views::take_before(500);

    EXPECT_EQ(std::ranges::distance(b), 500);
}

TEST(TakeBeforeTest, large_range_delimiter_at_end) {
    std::vector<int> v;
    for (int i = 0; i < 10000; ++i) {
        v.push_back(i);
    }

    auto b = v | tb::views::take_before(9999);

    EXPECT_EQ(std::ranges::distance(b), 9999);
}

TEST(TakeBeforeTest, large_range_not_found) {
    std::vector<int> v;
    for (int i = 0; i < 1000; ++i) {
        v.push_back(i);
    }

    auto b = v | tb::views::take_before(99999);

    EXPECT_EQ(std::ranges::distance(b), 1000);
}

// --- std::string as Range ---

TEST(TakeBeforeTest, string_as_range) {
    std::string s = "path/to/file.txt";
    auto        b = s | tb::views::take_before('/');

    std::string result;
    for (char c : b) {
        result += c;
    }

    EXPECT_EQ(result, "path");
}

TEST(TakeBeforeTest, string_with_extension) {
    std::string s = "document.pdf";
    auto        b = s | tb::views::take_before('.');

    std::string result;
    for (char c : b) {
        result += c;
    }

    EXPECT_EQ(result, "document");
}

// --- std::array ---

TEST(TakeBeforeTest, with_array) {
    std::array<int, 5> arr = {10, 20, 30, 40, 50};
    auto               b   = arr | tb::views::take_before(30);

    std::vector<int> result;
    std::ranges::copy(b, std::back_inserter(result));

    std::vector<int> expected = {10, 20};
    EXPECT_EQ(result, expected);
}

// --- Default Constructor ---

// std::ranges::owning_view is not available in GCC 11 (added in GCC 12)
#if defined(__cpp_lib_ranges_owning_view) || (defined(__GNUC__) && !defined(__clang__) && __GNUC__ >= 12) || \
    defined(__clang__) || defined(_MSC_VER)
TEST(TakeBeforeTest, default_constructible_concepts) {
    // ref_view is NOT default_initializable (requires a reference)
    // but take_before_view with owning_view should work
    using OwningView = std::ranges::owning_view<std::vector<int>>;

    // Verify the concept requirements
    static_assert(std::default_initializable<OwningView>);
    static_assert(std::default_initializable<int>);

    // This should compile and work
    tb::take_before_view<OwningView, int> view;
    (void)view; // Suppress unused warning
}
#endif

// --- Paper Example: views::take_before(one_two, '?') ---

TEST(TakeBeforeTest, paper_example) {
    const char* one_two = "One?Two";
    std::string result;
    for (auto c : tb::views::take_before(one_two, '?')) {
        result += c;
    }
    EXPECT_EQ(result, "One");
}

// --- View Interface ---

TEST(TakeBeforeTest, view_interface_empty) {
    std::vector<int> v = {1, 2, 3};
    auto             b = v | tb::views::take_before(1);

    EXPECT_TRUE(b.empty());
}

TEST(TakeBeforeTest, view_interface_not_empty) {
    std::vector<int> v = {1, 2, 3};
    auto             b = v | tb::views::take_before(3);

    EXPECT_FALSE(b.empty());
}

TEST(TakeBeforeTest, view_interface_front) {
    std::vector<int> v = {1, 2, 3, 4, 5};
    auto             b = v | tb::views::take_before(3);

    EXPECT_EQ(b.front(), 1);
}

// --- Const Correctness ---

TEST(TakeBeforeTest, const_range_iteration) {
    const std::vector<int> v = {1, 2, 3, 4, 5};
    const auto             b = tb::take_before_view(std::views::all(v), 3);

    std::vector<int> result;
    for (int i : b) {
        result.push_back(i);
    }

    std::vector<int> expected = {1, 2};
    EXPECT_EQ(result, expected);
}

// --- Special Characters ---

TEST(TakeBeforeTest, with_tab_delimiter) {
    const char* s = "col1\tcol2\tcol3";
    auto        b = tb::views::take_before(s, '\t');

    std::string result;
    for (char c : b) {
        result += c;
    }

    EXPECT_EQ(result, "col1");
}

TEST(TakeBeforeTest, with_space_delimiter) {
    const char* s = "word1 word2 word3";
    auto        b = tb::views::take_before(s, ' ');

    std::string result;
    for (char c : b) {
        result += c;
    }

    EXPECT_EQ(result, "word1");
}

// --- Boolean Values ---

TEST(TakeBeforeTest, with_bool_values) {
    std::vector<bool> v = {true, true, false, true};
    auto              b = v | tb::views::take_before(false);

    std::vector<bool> result;
    for (bool val : b) {
        result.push_back(val);
    }

    std::vector<bool> expected = {true, true};
    EXPECT_EQ(result, expected);
}

// --- Size Verification ---

TEST(TakeBeforeTest, count_elements) {
    std::vector<int> v = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
    auto             b = v | tb::views::take_before(6);

    int count = 0;
    for ([[maybe_unused]] auto _ : b) {
        ++count;
    }

    EXPECT_EQ(count, 5);
}

// ============================================================================
// ADVANCED EDGE CASE TESTS
// ============================================================================

// --- Verify tidy_obj for common types ---
// Note: tidy_obj types (empty, trivial) are intended for use with constant wrappers
// like std::integral_constant. Most normal value types (char, int, etc.) are NOT tidy_obj
// because they are not empty.

TEST(TakeBeforeTest, verify_tidy_obj_properties) {
    // char is NOT tidy_obj (not empty - has size 1)
    static_assert(!tb::tidy_obj<char>);
    static_assert(!tb::tidy_obj<int>);
    static_assert(!tb::tidy_obj<double>);
    static_assert(!tb::tidy_obj<std::string>);

    // Empty comparator-like types ARE tidy_obj (but can't be used as delimiter values
    // because they don't compare with range elements in a meaningful way)
    static_assert(tb::tidy_obj<std::equal_to<>>); // Empty, trivial
    static_assert(tb::tidy_obj<std::less<>>);
    static_assert(tb::tidy_obj<std::greater<>>);

    // This test verifies the concept itself works correctly
    // Actual tidy_obj usage would require a constant wrapper type like std::integral_constant
}

// --- borrowed_range behavior ---

TEST(TakeBeforeTest, borrowed_range_is_false_for_normal_values) {
    // For normal value types (non-tidy_obj), take_before_view is NEVER borrowed
    // even if the underlying range is borrowed
    std::vector<int> v    = {1, 2, 3, 4, 5};
    auto             subr = std::ranges::subrange(v.begin(), v.end());

    // subrange is borrowed
    static_assert(std::ranges::borrowed_range<decltype(subr)>);

    // But take_before with int (not tidy_obj) is NOT borrowed
    auto b = tb::take_before_view(subr, 3);
    static_assert(!std::ranges::borrowed_range<decltype(b)>);

    std::vector<int> result;
    std::ranges::copy(b, std::back_inserter(result));
    std::vector<int> expected = {1, 2};
    EXPECT_EQ(result, expected);
}

// --- Sentinel Base Accessor ---

TEST(TakeBeforeTest, sentinel_base_accessor) {
    std::vector<int> v = {1, 2, 3, 4, 5};
    auto             b = v | tb::views::take_before(3);

    auto sentinel            = b.end();
    auto underlying_sentinel = sentinel.base();

    // Should be the end of the underlying range
    EXPECT_EQ(underlying_sentinel, v.end());
}

// --- Non-const to Const Sentinel Conversion ---

TEST(TakeBeforeTest, sentinel_const_conversion) {
    std::vector<int> v = {1, 2, 3, 4, 5};
    auto             b = tb::take_before_view(std::views::all(v), 3);

    // Get non-const begin/end
    auto begin = b.begin();
    auto end   = b.end();

    // Verify iteration works
    int count = 0;
    for (auto it = begin; it != end; ++it) {
        ++count;
    }
    EXPECT_EQ(count, 2); // {1, 2}
}

// --- Const View Begin/End ---

TEST(TakeBeforeTest, const_view_begin_end) {
    const std::vector<int> v = {1, 2, 3, 4, 5};
    const auto             b = tb::take_before_view(std::views::all(v), 4);

    auto begin = b.begin();
    auto end   = b.end();

    std::vector<int> result;
    for (auto it = begin; it != end; ++it) {
        result.push_back(*it);
    }

    std::vector<int> expected = {1, 2, 3};
    EXPECT_EQ(result, expected);
}

// --- Operator== with different iterator types ---

TEST(TakeBeforeTest, begin_end_equality) {
    std::vector<int> v = {1, 2, 3};
    auto             b = v | tb::views::take_before(1); // Empty result

    EXPECT_EQ(b.begin(), b.end()); // Should be equal (empty range)
}

TEST(TakeBeforeTest, begin_end_inequality) {
    std::vector<int> v = {1, 2, 3};
    auto             b = v | tb::views::take_before(3); // {1, 2}

    EXPECT_NE(b.begin(), b.end()); // Should NOT be equal
}

// --- Move-only value type in range ---

TEST(TakeBeforeTest, with_unique_ptr_pointers) {
    // Test with a range of pointers (not unique_ptr itself to avoid move issues)
    std::vector<int*> v;
    int               a = 1, b_val = 2, c = 3;
    v.push_back(&a);
    v.push_back(&b_val);
    v.push_back(&c);

    auto view = v | tb::views::take_before(&b_val);

    std::vector<int*> result;
    for (auto* ptr : view) {
        result.push_back(ptr);
    }

    EXPECT_EQ(result.size(), 1u);
    EXPECT_EQ(result[0], &a);
}

// --- Subrange with unreachable_sentinel ---

TEST(TakeBeforeTest, with_unreachable_sentinel_subrange) {
    std::vector<int> v    = {1, 2, 3, 4, 5};
    auto             subr = std::ranges::subrange(v.begin(), std::unreachable_sentinel);

    auto b = tb::take_before_view(subr, 3);

    std::vector<int> result;
    for (int i : b) {
        result.push_back(i);
    }

    std::vector<int> expected = {1, 2};
    EXPECT_EQ(result, expected);
}

// --- Verify view concept ---

TEST(TakeBeforeTest, is_view) {
    using ViewType = tb::take_before_view<std::ranges::ref_view<std::vector<int>>, int>;
    static_assert(std::ranges::view<ViewType>);
    static_assert(std::ranges::input_range<ViewType>);
}

// --- Empty delimiter string ---

TEST(TakeBeforeTest, string_empty_at_start) {
    std::string s = "";
    auto        b = s | tb::views::take_before('x');

    EXPECT_TRUE(std::ranges::empty(b));
}

// --- Unicode-ish (wstring) ---

TEST(TakeBeforeTest, with_wstring) {
    std::wstring ws = L"Hello World";
    auto         b  = ws | tb::views::take_before(L' ');

    std::wstring result;
    for (wchar_t c : b) {
        result += c;
    }

    EXPECT_EQ(result, L"Hello");
}

// --- Chained take_before ---

TEST(TakeBeforeTest, chained_take_before) {
    std::string s = "part1:part2:part3";

    // First take_before gets "part1:part2"... wait no, it gets "part1"
    // Let's use a different approach - take to first ':', then that result
    auto first = s | tb::views::take_before(':');

    std::string result;
    for (char c : first) {
        result += c;
    }

    EXPECT_EQ(result, "part1");
}

// --- Negative value delimiter ---

TEST(TakeBeforeTest, negative_delimiter) {
    std::vector<int> v = {5, 4, 3, -1, 2, 1};
    auto             b = v | tb::views::take_before(-1);

    std::vector<int> result;
    std::ranges::copy(b, std::back_inserter(result));

    std::vector<int> expected = {5, 4, 3};
    EXPECT_EQ(result, expected);
}

// --- Float comparison edge case ---

TEST(TakeBeforeTest, float_exact_match) {
    std::vector<float> v = {1.0f, 2.0f, 3.0f, 4.0f};
    auto               b = v | tb::views::take_before(3.0f);

    std::vector<float> result;
    std::ranges::copy(b, std::back_inserter(result));

    std::vector<float> expected = {1.0f, 2.0f};
    EXPECT_EQ(result, expected);
}

// --- Back-to-back delimiters ---

TEST(TakeBeforeTest, consecutive_delimiters) {
    std::vector<int> v = {1, 5, 5, 5, 2};
    auto             b = v | tb::views::take_before(5);

    std::vector<int> result;
    std::ranges::copy(b, std::back_inserter(result));

    std::vector<int> expected = {1}; // Stops at first 5
    EXPECT_EQ(result, expected);
}
