// A combination of an unordered map and a priority queue
#pragma once

#include <functional>

namespace ds {
template <typename K, typename V, typename HASH = std::hash<K>,
          typename KEY_EQUAL = std::equal_to<K>,
          typename COMPARE = std::less<K>>
class kvpq {
  // TODO
  using size_type = std::size_t;
  using iterator = K *;
  using const_iterator = const K *;
  inline static constexpr size_type DEFAULT_SIZE = 10;

public:
  // (1)
  kvpq() : kvpq(DEFAULT_SIZE) {}
  explicit kvpq(size_type, const HASH & = HASH(),
                const KEY_EQUAL & = KEY_EQUAL(), const COMPARE & = COMPARE());
  // (2)
  template <typename IT>
  kvpq(IT b, IT e, size_type bucket_count = DEFAULT_SIZE, const HASH & = HASH(),
       const KEY_EQUAL & = KEY_EQUAL(), const COMPARE & = COMPARE());

  // (3)
  kvpq(const kvpq &);

  // (4)
  kvpq(kvpq &&);

  // (5)
  explicit kvpq(std::initializer_list<std::pair<K, V>> init,
                size_type bucket_count = DEFAULT_SIZE,
                const HASH &hash = HASH(),
                const KEY_EQUAL &key_equal = KEY_EQUAL(),
                const COMPARE &compare = COMPARE())
      : kvpq(init.begin(), init.end(), bucket_count, hash, key_equal, compare) {
  }

  ~kvpq();

  kvpq &operator=(const kvpq &);
  kvpq &operator=(kvpq &&);

  // Iterators
  iterator begin() noexcept;
  const_iterator begin() const noexcept;
  const_iterator cbegin() const noexcept { return begin(); }
  iterator end() noexcept;
  const_iterator end() const noexcept;
  const_iterator cend() const noexcept { return end(); }

  // Capacity
  [[nodiscard]] bool empty() const noexcept;
  size_type size() const noexcept;
  size_type max_size() const noexcept;

  // Modifiers
  void push(const V &);
  void push(V &&);
  void pop();
  void clear() noexcept;

  // insert(1)
  std::pair<iterator, bool> insert(const std::pair<K, V> &);
  std::pair<iterator, bool> insert(std::pair<K, V> &&);
  // insert(2)
  template <typename P> std::pair<iterator, bool> insert(P &&);
  // insert(3)
  iterator insert(const_iterator hint, const std::pair<K, V> &);
  iterator insert(const_iterator hint, std::pair<K, V> &&);
  // insert(4)
  template <typename P>
  std::pair<iterator, bool> insert(const_iterator hint, P &&);
  // insert(5)
  template <typename IT> void insert(IT first, IT last);
  // insert(6)
  void insert(std::initializer_list<std::pair<K, V>> init);

  // insert_or_assign(1)
  template <typename M>
  std::pair<iterator, bool> insert_or_assign(const K &, M &&);
  // insert_or_assign(2)
  template <typename M> std::pair<iterator, bool> insert_or_assign(K &&, M &&);
  // insert_or_assign(3)
  template <typename M>
  std::pair<iterator, bool> insert_or_assign(const_iterator hint, const K &,
                                             M &&);
  // insert_or_assign(4)
  template <typename M>
  std::pair<iterator, bool> insert_or_assign(const_iterator hint, K &&, M &&);

  template <typename... ARGS> std::pair<iterator, bool> emplace(ARGS &&...);
  template <typename... ARGS>
  iterator emplace_hint(const_iterator hint, ARGS &&...);

  // try_emplace(1)
  template <typename... ARGS>
  std::pair<iterator, bool> try_emplace(const K &, ARGS &&...);
  // try_emplace(2)
  template <typename... ARGS>
  std::pair<iterator, bool> try_emplace(K &&, ARGS &&...);
  // try_emplace(3)
  template <typename... ARGS>
  std::pair<iterator, bool> try_emplace(const_iterator hint, const K &,
                                        ARGS &&...);
  // try_emplace(4)
  template <typename... ARGS>
  std::pair<iterator, bool> try_emplace(const_iterator hint, K &&, ARGS &&...);

  iterator erase(const_iterator pos);
  iterator erase(const_iterator first, const_iterator last);
  size_type erase(const K &);
  void swap(kvpq &);

  // mege(1)
  template <typename H2, typename P2, typename C2>
  void merge(const kvpq<K, V, H2, P2, C2> &);
  // mege(2)
  template <typename H2, typename P2, typename C2>
  void merge(kvpq<K, V, H2, P2, C2> &&);

  // Lookup
  const std::pair<K, V> &top() const;
  V &at(const K &);
  const V &at(const K &) const;
  V &operator[](const K &);
  V &operator[](K &&);
  size_type count(const K &) const;

  iterator find(const K &);
  const_iterator find(const K &) const;
  bool contains(const K &) const;

  // Hash policy
  float load_factor() const;
  float max_load_factor() const;
  void max_load_factor(float lf);
  void rehash(size_type bucket_count);
  void reserve(size_type count);

  // Observers
  HASH hash_function() const;
  KEY_EQUAL key_eq() const;
  COMPARE comp() const;

  // Non-member functions
  friend bool operator==(const kvpq &, const kvpq &);
  friend bool operator!=(const kvpq &lhs, const kvpq &rhs) {
    return !(lhs == rhs);
  }
  friend void swap(const kvpq &lhs, const kvpq &rhs) { return lhs.swap(rhs); }

private:
};
} // namespace ds
