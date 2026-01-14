#include <cstddef>

namespace edenlib {
template <class T> class allocator {
  using value_type = T;

public:
  constexpr allocator() noexcept {}
  constexpr allocator(const allocator &other) noexcept {}
  constexpr ~allocator() {}

  constexpr T *allocate(std::size_t n) {
    return static_cast<T *>(operator new(n * sizeof(T)));
  }
  constexpr void deallocate(T *p, std::size_t n) { operator delete(p, n); }
};

} // namespace edenlib
