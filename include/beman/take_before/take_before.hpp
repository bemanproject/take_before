// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

#ifndef BEMAN_TAKE_BEFORE_TAKE_BEFORE_HPP
#define BEMAN_TAKE_BEFORE_TAKE_BEFORE_HPP

#include <algorithm>
#include <concepts>
#include <memory>
#include <optional>
#include <ranges>
#include <type_traits>
#include <utility>

namespace beman::take_before {

// ============================================================================
// Exposition-only helpers (not in C++20 standard library)
// ============================================================================

// [range.utility.helpers] maybe-const - exposition only
template <bool Const, class T>
using maybe_const = std::conditional_t<Const, const T, T>;

// [range.utility.helpers] simple-view - exposition only
template <class R>
concept simple_view = std::ranges::view<R> && std::ranges::range<const R> &&
                      std::same_as<std::ranges::iterator_t<R>, std::ranges::iterator_t<const R>> &&
                      std::same_as<std::ranges::sentinel_t<R>, std::ranges::sentinel_t<const R>>;

// ============================================================================
// tidy-obj concept for borrowed range optimization
// ============================================================================

// [range.take.before], take before view
template <class T>
constexpr bool tidy_obj = // exposition only
    std::is_empty_v<T> && std::is_trivially_default_constructible_v<T> && std::is_trivially_destructible_v<T>;

// Using std::optional as a substitute for movable-box in this implementation.
template <typename T>
using movable_box = std::optional<T>;

// ============================================================================
// take_before_view class template
// ============================================================================

template <std::ranges::view V, std::move_constructible T>
    requires std::ranges::input_range<V> && std::is_object_v<T> &&
             std::indirect_binary_predicate<std::ranges::equal_to, std::ranges::iterator_t<V>, const T*>
class take_before_view : public std::ranges::view_interface<take_before_view<V, T>> {
    template <bool>
    class sentinel; // exposition only

    V              base_ = V(); // exposition only
    movable_box<T> value_;      // exposition only

  public:
    take_before_view()
        requires std::default_initializable<V> && std::default_initializable<T>
    = default;

    constexpr explicit take_before_view(V base, const T& value)
        requires std::copy_constructible<T>
        : base_(std::move(base)), value_(value) {}

    constexpr explicit take_before_view(V base, T&& value) : base_(std::move(base)), value_(std::move(value)) {}

    constexpr V base() const&
        requires std::copy_constructible<V>
    {
        return base_;
    }
    constexpr V base() && { return std::move(base_); }

    constexpr auto begin()
        requires(!simple_view<V>)
    {
        return std::ranges::begin(base_);
    }

    constexpr auto begin() const
        requires std::ranges::range<const V> &&
                 std::indirect_binary_predicate<std::ranges::equal_to, std::ranges::iterator_t<const V>, const T*>
    {
        return std::ranges::begin(base_);
    }

    constexpr auto end()
        requires(!simple_view<V>)
    {
        if constexpr (tidy_obj<T>)
            return sentinel<false>(std::ranges::end(base_));
        else
            return sentinel<false>(std::ranges::end(base_), std::addressof(*value_));
    }

    constexpr auto end() const
        requires std::ranges::range<const V> &&
                 std::indirect_binary_predicate<std::ranges::equal_to, std::ranges::iterator_t<const V>, const T*>
    {
        if constexpr (tidy_obj<T>)
            return sentinel<true>(std::ranges::end(base_));
        else
            return sentinel<true>(std::ranges::end(base_), std::addressof(*value_));
    }
};

// ============================================================================
// take_before_view::sentinel class template
// ============================================================================

template <std::ranges::view V, std::move_constructible T>
    requires std::ranges::input_range<V> && std::is_object_v<T> &&
             std::indirect_binary_predicate<std::ranges::equal_to, std::ranges::iterator_t<V>, const T*>
template <bool Const>
class take_before_view<V, T>::sentinel {
    using Base = maybe_const<Const, V>; // exposition only

    std::ranges::sentinel_t<Base> end_   = std::ranges::sentinel_t<Base>(); // exposition only
    const T*                      value_ = nullptr; // exposition only, present only if tidy-obj<T> is false

    template <bool>
    friend class sentinel;

    // Private constructors (exposition only)
    constexpr sentinel(std::ranges::sentinel_t<Base> end, const T* value)
        requires(!tidy_obj<T>)
        : end_(end), value_(value) {}

    constexpr sentinel(std::ranges::sentinel_t<Base> end)
        requires tidy_obj<T>
        : end_(end) {}

    friend class take_before_view;

  public:
    sentinel() = default;

    constexpr sentinel(sentinel<!Const> s)
        requires Const && std::convertible_to<std::ranges::sentinel_t<V>, std::ranges::sentinel_t<Base>>
        : end_(std::move(s.end_)) {
        if constexpr (!tidy_obj<T>) {
            value_ = s.value_;
        }
    }

    constexpr std::ranges::sentinel_t<Base> base() const { return end_; }

    friend constexpr bool operator==(const std::ranges::iterator_t<Base>& x, const sentinel& y) {
        if constexpr (tidy_obj<T>) {
            return y.end_ == x || T() == *x;
        } else {
            return y.end_ == x || *y.value_ == *x;
        }
    }

    template <bool OtherConst = !Const>
        requires std::sentinel_for<std::ranges::sentinel_t<Base>, std::ranges::iterator_t<maybe_const<OtherConst, V>>>
    friend constexpr bool operator==(const std::ranges::iterator_t<maybe_const<OtherConst, V>>& x, const sentinel& y) {
        if constexpr (tidy_obj<T>) {
            return y.end_ == x || T() == *x;
        } else {
            return y.end_ == x || *y.value_ == *x;
        }
    }
};

// CTAD
template <class R, class T>
take_before_view(R&&, T) -> take_before_view<std::ranges::views::all_t<R>, T>;

} // namespace beman::take_before

// ============================================================================
// enable_borrowed_range specialization
// ============================================================================

namespace std::ranges {
template <class V, class T>
constexpr bool enable_borrowed_range<beman::take_before::take_before_view<V, T>> =
    enable_borrowed_range<V> && beman::take_before::tidy_obj<T>;
} // namespace std::ranges

// ============================================================================
// views::take_before adaptor
// ============================================================================

namespace beman::take_before::views {

namespace detail {

// Range adaptor closure for pipe operator (C++20 compatible implementation)
template <class T>
class take_before_closure {
    T value_;

  public:
    constexpr explicit take_before_closure(T value) : value_(std::move(value)) {}

    template <std::ranges::viewable_range R>
        requires requires { beman::take_before::take_before_view(std::declval<R>(), std::declval<T>()); }
    constexpr auto operator()(R&& r) const {
        return beman::take_before::take_before_view(std::forward<R>(r), value_);
    }

    // Pipe operator
    template <std::ranges::viewable_range R>
        requires requires { beman::take_before::take_before_view(std::declval<R>(), std::declval<T>()); }
    friend constexpr auto operator|(R&& r, const take_before_closure& self) {
        return self(std::forward<R>(r));
    }
};

} // namespace detail

struct take_before_fn {
    // Overload 1: viewable_range
    template <std::ranges::viewable_range R, typename T>
        requires requires { beman::take_before::take_before_view(std::declval<R>(), std::declval<T>()); }
    constexpr auto operator()(R&& r, T&& value) const {
        return beman::take_before::take_before_view(std::forward<R>(r), std::forward<T>(value));
    }

    // Overload 2: input_iterator (not range)
    template <std::input_iterator I, typename T>
        requires(!std::ranges::range<I>) && requires {
            beman::take_before::take_before_view(std::ranges::subrange(std::declval<I>(), std::unreachable_sentinel),
                                                 std::declval<T>());
        }
    constexpr auto operator()(I i, T&& value) const {
        return beman::take_before::take_before_view(std::ranges::subrange(i, std::unreachable_sentinel),
                                                    std::forward<T>(value));
    }

    // Overload 3: single argument for pipe operator
    template <typename T>
    constexpr auto operator()(T&& value) const {
        return detail::take_before_closure<std::decay_t<T>>(std::forward<T>(value));
    }
};

inline constexpr take_before_fn take_before;

} // namespace beman::take_before::views

#endif // BEMAN_TAKE_BEFORE_TAKE_BEFORE_HPP
