#pragma once

namespace eden {

template <class Diff_first, class Diff_second> struct is_same_struct {
  static constexpr bool value = false;
};

template <class Same> struct is_same_struct<Same, Same> {
  static constexpr bool value = true;
};

template <class T> struct remove_const_struct {
  typedef T type;
};

template <class T> struct remove_const_struct<const T> {
  typedef T type;
};

template <class T> struct remove_volatile_struct {
  typedef T type;
};

template <class T> struct remove_volatile_struct<volatile T> {
  typedef T type;
};

template <class T> using remove_const = remove_const_struct<T>::type;
template <class T> using remove_volatile = remove_volatile_struct<T>::type;
template <class T> using remove_cv = remove_const<remove_volatile<T>>;

template <class T> struct remove_ref_struct {
  typedef T type;
};

template <class T> struct remove_ref_struct<T &> {
  typedef T type;
};

template <class T> struct remove_ref_struct<T &&> {
  typedef T type;
};

template <class T> using remove_ref = remove_ref_struct<T>::type;
template <class T> using remove_cvref = remove_ref<remove_cv<T>>;
template <class T> using add_rval_ref = T &&;
template <class T> using add_lval_ref = T &;

template <class T> add_rval_ref<T> declval() noexcept {
  static_assert(false, "declval not allowed in evaluated contexts");
}

template <class T> struct difference_type_struct {
  typedef remove_ref<decltype(declval<T>() - declval<T>())> type;
};

template <class T> using difference_type = difference_type_struct<T>::type;

} // namespace eden
