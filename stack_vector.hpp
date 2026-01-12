#include <cstddef>
#include <cstdint>
#include <memory>

template <class T> class DefaultAllocator {
  using value_type = T;

public:
  constexpr DefaultAllocator() noexcept {}
  constexpr DefaultAllocator(const DefaultAllocator &other) noexcept {}
  constexpr ~DefaultAllocator() {}

  constexpr T *allocate(std::size_t n) { return operator new(n * sizeof(T)); }
  constexpr void deallocate(T *p, std::size_t n) { operator delete(p, n); }
};

template <class T, std::size_t StackBufferSize,
          class Allocator = DefaultAllocator<T>>
class StackVector {
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
  static constexpr size_t value_size = sizeof(T);

  [[no_unique_address]] Allocator m_alloc;
  size_type m_end{0};
  std::byte m_stack_buffer[StackBufferSize * value_size];
  T *m_begin_heap{nullptr};
  T *m_heap_capacity_end{nullptr};

public:
  /* Special Member Functions */
  explicit constexpr StackVector() noexcept(noexcept(Allocator()))
      : StackVector(Allocator()) {}

  explicit constexpr StackVector(const Allocator &alloc) : m_alloc(alloc) {}
  explicit StackVector(size_type count, const Allocator &alloc = Allocator())
      : m_alloc(alloc) {
    const size_type overflow =
        count > StackBufferSize ? count - StackBufferSize : 0;
    const size_type num_on_stack =
        count > StackBufferSize ? StackBufferSize : count;
    for (; m_end < num_on_stack; ++m_end)
      std::construct_at(&m_stack_buffer[m_end * value_size]);

    if (overflow) {
      m_begin_heap = m_alloc.allocate(overflow);
      for (size_type i{}; i < overflow; ++i) {
        std::construct_at(&m_begin_heap[i]);
        ++m_end;
      }
      m_heap_capacity_end = m_begin_heap + overflow;
    }
  }
  constexpr StackVector(size_type count, const T &value,
                        const Allocator &alloc = Allocator()) {
    const size_type overflow =
        count > StackBufferSize ? count - StackBufferSize : 0;
    const size_type num_on_stack =
        count > StackBufferSize ? StackBufferSize : count;
    for (; m_end < num_on_stack; ++m_end)
      std::construct_at(&m_stack_buffer[m_end * value_size], value);

    if (overflow) {
      m_begin_heap = m_alloc.allocate(overflow);
      for (size_type i{}; i < overflow; ++i) {
        std::construct_at(&m_begin_heap[i], value);
        ++m_end;
      }
      m_heap_capacity_end = m_begin_heap + overflow;
    }
  }
  StackVector(std::initializer_list<T> init,
              const Allocator &alloc = Allocator());

  constexpr StackVector(const StackVector &other) : m_alloc(other.m_alloc) {
    const size_type overflow =
        other.m_end > StackBufferSize ? other.m_end - StackBufferSize : 0;
    const size_type num_on_stack = std::min(other.m_end, StackBufferSize);

    for (size_type i{}; i < num_on_stack; ++i)
      std::construct_at(&m_stack_buffer[i], other.m_stack_buffer[i]);

    if (overflow) {
      m_begin_heap = m_alloc(overflow);
      for (size_type i{}; i < overflow; ++i) {
        std::construct_at(&m_begin_heap[i], other.m_begin_heap[i]);
        ++m_end;
      }
      m_heap_capacity_end = m_begin_heap + overflow;
    }
  };

  constexpr StackVector(StackVector &&other) noexcept
      : m_alloc(std::move(other.m_alloc)), m_end(other.m_end),
        m_begin_heap(other.m_begin_heap),
        m_heap_capacity_end(other.m_heap_capacity_end) {
    const size_type num_on_stack = std::min(other.m_end, StackBufferSize);

    for (size_type i{}; i < num_on_stack; ++i)
      std::construct_at(&m_stack_buffer[i],
                        std::move_if_noexcept(other.m_stack_buffer[i]));

    other.m_begin_heap = nullptr;
    other.m_heap_capacity_end = nullptr;
  }

  constexpr ~StackVector();

  constexpr StackVector &operator=(const StackVector &other);
  constexpr StackVector &operator=(StackVector &&other) noexcept;
  constexpr StackVector &operator=(std::initializer_list<T> ilist) noexcept;
  /* Special Member Functions */

  /* Element Access */
  constexpr T &at(size_type pos);
  constexpr const T &at(size_type pos) const;
  constexpr T &operator[](size_type pos);
  constexpr const T &operator[](size_type pos) const;

  constexpr T &front();
  constexpr const T &front() const;

  constexpr T &back();
  constexpr const T &back() const;

  constexpr T *stack_data() noexcept;
  constexpr T *heap_data() noexcept;
  constexpr const T *stack_data() const noexcept;
  constexpr const T *heap_data() const noexcept;
  /* Element Access */

  /* Capacity */
  constexpr bool is_empty() const noexcept;
  constexpr size_type size() const noexcept;
  constexpr void reserve(size_type new_cap);
  constexpr size_type capacity() const noexcept;
  /* Capacity */

  /* Modifiers */
  constexpr void clear() noexcept;
  constexpr void push_back(const T &value);
  constexpr void push_back(T &&value);
  constexpr void pop_back();
  constexpr void resize(size_type count);
  /* Modifiers */
};
