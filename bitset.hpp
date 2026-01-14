#include <cstddef>
#include <limits>
#include <stdexcept>
#include <string>

namespace edenlib {
template <size_t N> struct Bitset;

template <size_t N>
constexpr Bitset<N> operator&(const Bitset<N> &lhs,
                              const Bitset<N> &rhs) noexcept {
  Bitset<N> ret_set;
  for (auto i{0uz}; i < Bitset<N>::num_data; ++i)
    ret_set.bits[i] = lhs.bits[i] & rhs.bits[i];

  return ret_set;
}

template <size_t N>
constexpr Bitset<N> operator|(const Bitset<N> &lhs,
                              const Bitset<N> &rhs) noexcept {
  Bitset<N> ret_set;
  for (auto i{0uz}; i < Bitset<N>::num_data; ++i)
    ret_set.bits[i] = lhs.bits[i] | rhs.bits[i];

  return ret_set;
}

template <size_t N>
constexpr Bitset<N> operator^(const Bitset<N> &lhs,
                              const Bitset<N> &rhs) noexcept {
  Bitset<N> ret_set;
  for (auto i{0uz}; i < Bitset<N>::num_data; ++i)
    ret_set.bits[i] = lhs.bits[i] ^ rhs.bits[i];

  return ret_set;
}

/* To Do:
 *  Add bitshift functionality
 *  Add bit-reference
 */
template <size_t N> struct Bitset {

  using data_type = unsigned long long;

  static constexpr size_t sizeofType = sizeof(data_type);
  static constexpr size_t num_data = (N - 1) / sizeofType + 1;
  static constexpr data_type maxofType = std::numeric_limits<data_type>::max();
  static constexpr size_t indexOf(size_t pos) noexcept {
    return pos / sizeofType;
  }
  static constexpr size_t size() noexcept { return N; }

  static constexpr data_type maskFor(size_t pos) noexcept {
    return 1ull << (pos % sizeofType);
  }

  constexpr bool operator[](size_t pos) const { return test(pos); }

  data_type bits[num_data];

  constexpr bool test(size_t pos) const {
    if (indexOf(pos) > (num_data - 1))
      throw std::runtime_error("Invalid pos for bitset");

    return bits[indexOf(pos)] & maskFor(pos);
  }

  constexpr bool all() const {

    for (auto i{0uz}; i < N; ++i) {
      if (!test(i))
        return false;
    }

    return true;
  }

  constexpr bool any() const {
    for (auto i{0uz}; i < N; ++i) {
      if (test(i))
        return true;
    }

    return false;
  }

  constexpr bool none() const { return !all(); }

  constexpr size_t count() const {
    size_t num_true{};
    for (auto i{0uz}; i < N; ++i) {
      if (test(i))
        ++num_true;
    }

    return num_true;
  }

  friend constexpr Bitset operator&
      <>(const Bitset &lhs, const Bitset &rhs) noexcept;
  friend constexpr Bitset operator|
      <>(const Bitset &lhs, const Bitset &rhs) noexcept;
  friend constexpr Bitset operator^
      <>(const Bitset &lhs, const Bitset &rhs) noexcept;

  constexpr Bitset &operator&=(const Bitset &other) noexcept {
    *this = *this & other;
    return *this;
  }

  constexpr Bitset &operator|=(const Bitset &other) noexcept {
    *this = *this | other;
    return *this;
  }

  constexpr Bitset &operator^=(const Bitset &other) noexcept {
    *this = *this ^ other;
    return *this;
  }

  constexpr Bitset operator~() const noexcept {
    Bitset ret_val = *this;
    ret_val.flip();
    return ret_val;
  }

  constexpr Bitset &set() {
    for (auto &num : bits)
      num = maxofType;

    return *this;
  }

  constexpr Bitset &set(size_t pos, bool value = true) {
    if (value)
      bits[indexOf(pos)] |= maskFor(pos);
    else
      bits[indexOf(pos)] &= (~maskFor(pos));

    return *this;
  }

  constexpr Bitset &flip() noexcept {
    for (auto i{0uz}; i < N; ++i)
      set(i, !test(i));

    return *this;
  }

  constexpr Bitset &flip(size_t pos) {
    set(pos, !test(pos));
    return *this;
  }

  constexpr std::string to_string() {
    std::string ret_val;

    // technically doesn't work if you want >9 quintibytes of data
    for (int i{N - 1}; i >= 0; --i) {
      const char bit = test(i) ? '1' : '0';
      ret_val.push_back(bit);
    }

    return ret_val;
  }

  constexpr Bitset() noexcept {
    for (auto &num : bits)
      num = 0;
  }

  constexpr Bitset(std::string str) : Bitset() {
    for (auto i{0uz}; i < N; ++i) {
      if (str.empty())
        return;

      const bool bit = str.back() == '1' ? true : false;
      set(i, bit);
      str.pop_back();
    }
  }

  constexpr Bitset(const Bitset &other) {
    for (auto i{0uz}; i < num_data; ++i)
      bits[i] = other.bits[num_data];
  }
};

} // namespace edenlib
