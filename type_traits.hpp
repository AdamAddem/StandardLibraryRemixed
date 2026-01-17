#pragma once

namespace eden {

template <class Diff_first, class Diff_second> struct is_same_struct {
  static constexpr bool value = false;
};

template <class Same> struct is_same_struct<Same, Same> {
  static constexpr bool value = true;
};

template <class T> struct remove_cv_struct {
  typedef T type;
};

template <class T> struct remove_cv_struct<const T> {
  typedef T type;
};

template <class T> struct remove_cv_struct<volatile T> {
  typedef T type;
};

template <class T> struct remove_cv_struct<const volatile T> {
  typedef T type;
};

template <class T> using remove_cv = remove_cv_struct<T>::type;

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

} // namespace eden
