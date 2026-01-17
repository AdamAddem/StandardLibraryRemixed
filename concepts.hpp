#pragma once
#include "type_traits.hpp"
#include <concepts>
#include <type_traits> // stupid i hate it
#include <utility>

namespace eden {

template <class First, class Second>
concept same_c = is_same_struct<First, Second>::value;

template <class From, class To>
concept convertible_c = requires(From a) {
  [] -> To { return std::forward<From>(a); };
} && requires { static_cast<To>(std::declval<From>()); };

template <class From, class To>
concept nothrow_convertible_c = requires(From a) {
  {
    [] -> To { return std::forward<From>(a); }
  } noexcept;
} && requires {
  { static_cast<To>(std::declval<From>()) } noexcept;
};

template <class T>
concept destructible_c = requires(T a) { a.~T(); };

template <class T>
concept nothrow_destructible_c = requires(T a) {
  { a.~T() } noexcept;
};

template <class T, class... Args>
concept constructible_from_c =
    nothrow_destructible_c<T> && requires { new T(std::declval<Args>()...); };

template <class T>
concept move_constructible_c =
    constructible_from_c<T, T> && convertible_c<T, T>;

template <class T>
concept lvalue_ref_c = same_c<T, remove_ref<T> &>;

template <class T>
concept rvalue_ref_c = same_c<T, remove_ref<T> &&>;

template <class T>
concept non_ref_c = same_c<T, remove_ref<T>>;

// very hard to implement manually.
template <class T, class U>
concept common_reference_with = std::common_reference_with<T, U>;

template <class LHS, class RHS>
concept assignable_from_c =
    lvalue_ref_c<LHS> &&
    common_reference_with<const remove_ref<LHS> &, const remove_ref<RHS> &> &&
    requires(LHS left, RHS &&right) {
      { left = std::forward<RHS>(right) } -> same_c<LHS>;
    };

// unfortunately std::is_union and std::is_enum are not possible to be
// implemented w/o compiler magic
template <class T>
concept union_c = std::is_union_v<T>;

template <class T>
concept enum_c = std::is_enum_v<T>;

template <class T>
concept class_c = !union_c<T> && requires(int T::*) { 0; };

template <class T>
concept fundamental_c = !class_c<T> && !union_c<T> && !enum_c<T>;

template <class T>
concept array_c = requires(T &a) { [](auto(&)[]) {}(a); } ||
                  requires(T &a) { []<std::size_t N>(auto(&)[N]) {}(a); };

template <class T>
concept pointer_c = fundamental_c<T> && !array_c<T> && requires(T a) { *a; };

template <class T>
concept member_pointer_c = requires(T a) {
  []<typename Class, typename Member>(Class Member::*) {}(a);
};

template <class T>
concept null_pointer_c = same_c<T, decltype(nullptr)>;

template <class T>
concept integral_c = fundamental_c<T> && requires(T a, T *p) { p + a; };

template <class T>
concept floating_point_c =
    !integral_c<T> && fundamental_c<T> && requires(T a, T b) { a + b; };

template <class T>
concept arithmetic_c = integral_c<T> || floating_point_c<T>;

template <class T>
concept scalar_c = arithmetic_c<T> || enum_c<T> || pointer_c<T> ||
                   member_pointer_c<T> || null_pointer_c<T>;

template <class T>
concept object_c = scalar_c<T> || array_c<T> || union_c<T> || class_c<T>;

template <class T>
concept moveable_c = true;

} // namespace eden
