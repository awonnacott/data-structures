// A combination of an unordered map and a priority queue
#pragma once

#include <functional>       // equal_to, hash, less
#include <initializer_list> // initializer_list
#include <utility>          // move, pair

namespace ds {
using std::forward;
using std::move;
template <typename KVPQ>
using kvpq_iterator =
    std::pair<typename KVPQ::key_type, typename KVPQ::mapped_type> *;
template <typename KVPQ>
using kvpq_const_iterator =
    const std::pair<typename KVPQ::key_type, typename KVPQ::mapped_type> *;

template <typename K, typename V, typename H = std::hash<K>,
          typename EQ = std::equal_to<K>, typename C = std::less<K>>
class kvpq {
  // TODO

public:
  using size_type = std::size_t;
  using key_type = K;
  using value_type = std::pair<K, V>;
  using mapped_type = V;
  using iterator = kvpq_iterator<kvpq>;
  using const_iterator = kvpq_const_iterator<kvpq>;
  inline static constexpr size_type DEFAULT_SIZE = 10;
  // (1)
  kvpq() : kvpq(DEFAULT_SIZE) {}
  explicit kvpq(size_type bucket_count, const H & = H(), const EQ & = EQ(),
                const C & = C());
  // (2)
  template <typename IT>
  kvpq(IT b, IT e, size_type bucket_count = DEFAULT_SIZE, const H & = H(),
       const EQ & = EQ(), const C & = C());

  // (3)
  kvpq(const kvpq &);

  // (4)
  kvpq(kvpq &&);

  // (5)
  explicit kvpq(std::initializer_list<std::pair<K, V>> init,
                size_type bucket_count = DEFAULT_SIZE, const H &hash = H(),
                const EQ &key_equal = EQ(), const C &comp = C())
      : kvpq(init.begin(), init.end(), bucket_count, hash, key_equal, comp) {}

  ~kvpq();

  kvpq &operator=(const kvpq &);
  kvpq &operator=(kvpq &&);

  // Iterators
  iterator begin() noexcept { return iterator(*this, 0); }
  const_iterator begin() const noexcept { return const_iterator(*this, 0); }
  const_iterator cbegin() const noexcept { return begin(); }
  iterator end() noexcept { return iterator(*this, _bucket_count); }
  const_iterator end() const noexcept {
    return const_iterator(*this, _bucket_count);
  }
  const_iterator cend() const noexcept { return end(); }

  // Capacity
  [[nodiscard]] bool empty() const noexcept { return _size == 0; }
  size_type size() const noexcept { return _size; }
  size_type max_size() const noexcept { return -1; }

  // Modifiers
  void push(const std::pair<K, V> &p) { insert(p); }
  void push(std::pair<K, V> &&p) { insert(p); }
  void pop();
  void clear() noexcept;

  // insert(1)
  std::pair<iterator, bool> insert(const std::pair<K, V> &);
  std::pair<iterator, bool> insert(std::pair<K, V> &&);
  // insert(2)
  template <typename P> std::pair<iterator, bool> insert(P &&p) {
    return emplace(forward(p));
  }
  // insert(3)
  iterator insert(const_iterator /* hint */, const std::pair<K, V> &p) {
    return insert(p);
  }
  iterator insert(const_iterator /* hint */, std::pair<K, V> &&p) {
    return insert(move(p));
  }
  // insert(4)
  template <typename P>
  std::pair<iterator, bool> insert(const_iterator /* hint */, P &&p) {
    return insert(forward(p));
  }
  // insert(5)
  template <typename IT> void insert(IT first, IT last);
  // insert(6)
  void insert(std::initializer_list<std::pair<K, V>> init) {
    insert(init.begin(), init.end());
  }

  // insert_or_assign(1)
  template <typename M>
  std::pair<iterator, bool> insert_or_assign(const K &, M &&);
  // insert_or_assign(2)
  template <typename M> std::pair<iterator, bool> insert_or_assign(K &&, M &&);
  // insert_or_assign(3)
  template <typename M>
  std::pair<iterator, bool> insert_or_assign(const_iterator /* hint */,
                                             const K &k, M &&v) {
    return insert_or_assign(k, forward(v));
  }
  // insert_or_assign(4)
  template <typename M>
  std::pair<iterator, bool> insert_or_assign(const_iterator /* hint */, K &&k,
                                             M &&v) {
    return insert_or_assign(move(k), forward(v));
  }

  template <typename... ARGS> std::pair<iterator, bool> emplace(ARGS &&...);
  template <typename... ARGS>
  iterator emplace_hint(const_iterator /* hint */, ARGS &&... args) {
    return emplace(forward(args)...).first;
  }

  // try_emplace(1)
  template <typename... ARGS>
  std::pair<iterator, bool> try_emplace(const K &, ARGS &&...);
  // try_emplace(2)
  template <typename... ARGS>
  std::pair<iterator, bool> try_emplace(K &&, ARGS &&...);
  // try_emplace(3)
  template <typename... ARGS>
  std::pair<iterator, bool> try_emplace(const_iterator /* hint */, const K &k,
                                        ARGS &&... args) {
    return try_emplace(k, forward(args)...);
  }
  // try_emplace(4)
  template <typename... ARGS>
  std::pair<iterator, bool> try_emplace(const_iterator /* hint */, K &&k,
                                        ARGS &&... args) {
    return try_emplace(move(k), forward(args)...);
  }

  iterator erase(const_iterator pos);
  iterator erase(const_iterator first, const_iterator last);
  size_type erase(const K &);
  void swap(kvpq &);

  // merge(1)
  template <typename H2, typename P2, typename C2>
  void merge(const kvpq<K, V, H2, P2, C2> &);
  // merge(2)
  template <typename H2, typename P2, typename C2>
  void merge(kvpq<K, V, H2, P2, C2> &&);

  // Lookup
  const std::pair<K, V> &top() const;
  V &at(const K &);
  const V &at(const K &) const;
  V &operator[](const K &);
  V &operator[](K &&);
  size_type count(const K &k) const { return contains(k); }

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
  H hash_function() const { return _hash; }
  EQ key_eq() const { return _key_equal; }
  C comp() const { return _comp; }

  // Non-member functions
  friend bool operator==(const kvpq &, const kvpq &);
  friend bool operator!=(const kvpq &lhs, const kvpq &rhs) {
    return !(lhs == rhs);
  }
  friend void swap(const kvpq &lhs, const kvpq &rhs) { return lhs.swap(rhs); }

private:
  size_type _bucket_count;
  [[no_unique_address]] H _hash;
  [[no_unique_address]] EQ _key_equal;
  [[no_unique_address]] C _comp;
  size_type _size = 0;
};

// (1)
template <typename K, typename V, typename H, typename EQ, typename C>
kvpq<K, V, H, EQ, C>::kvpq(size_type bucket_count, const H &hash,
                           const EQ &key_equal, const C &comp)
    : _bucket_count(bucket_count), _hash(hash), _key_equal(key_equal),
      _comp(comp) {
  // TODO
}
// (2)
template <typename K, typename V, typename H, typename EQ, typename C>
template <typename IT>
kvpq<K, V, H, EQ, C>::kvpq(IT b, IT e, size_type bucket_count, const H &hash,
                           const EQ &key_equal, const C &comp)
    : kvpq(bucket_count, hash, key_equal, comp) {
  // TODO
}
// (3)
template <typename K, typename V, typename H, typename EQ, typename C>
kvpq<K, V, H, EQ, C>::kvpq(const kvpq &o)
    : _bucket_count(o._bucket_count), _hash(o._hash), _key_equal(o._key_equal),
      _comp(o._comp) {
  // TODO
}

// (4)
template <typename K, typename V, typename H, typename EQ, typename C>
kvpq<K, V, H, EQ, C>::kvpq(kvpq &&o)
    : _bucket_count(o._bucket_count), _hash(move(o._hash)),
      _key_equal(move(o._key_equal)), _comp(move(o._comp)) {
  // TODO
}

template <typename K, typename V, typename H, typename EQ, typename C>
kvpq<K, V, H, EQ, C>::~kvpq() {
  // TODO
}

template <typename K, typename V, typename H, typename EQ, typename C>
kvpq<K, V, H, EQ, C> &kvpq<K, V, H, EQ, C>::operator=(const kvpq &o) {
  _bucket_count = o._bucket_count;
  _hash = o._hash;
  _key_equal = o._key_equal;
  _comp = o._comp;
  _size = o._size;
  // TODO
}

template <typename K, typename V, typename H, typename EQ, typename C>
kvpq<K, V, H, EQ, C> &kvpq<K, V, H, EQ, C>::operator=(kvpq &&o) {
  _bucket_count = o._bucket_count;
  _hash = move(o._hash);
  _key_equal = move(o._key_equal);
  _comp = move(o._comp);
  _size = o._size;
  // TODO
}

// Modifiers
template <typename K, typename V, typename H, typename EQ, typename C>
void kvpq<K, V, H, EQ, C>::pop() {
  // TODO
}
template <typename K, typename V, typename H, typename EQ, typename C>
void kvpq<K, V, H, EQ, C>::clear() noexcept {
  // TODO
}

// insert(1)
template <typename K, typename V, typename H, typename EQ, typename C>
std::pair<kvpq_iterator<kvpq<K, V, H, EQ, C>>, bool>
kvpq<K, V, H, EQ, C>::insert(const std::pair<K, V> &p) {
  // TODO
}
template <typename K, typename V, typename H, typename EQ, typename C>
std::pair<kvpq_iterator<kvpq<K, V, H, EQ, C>>, bool>
kvpq<K, V, H, EQ, C>::insert(std::pair<K, V> &&p) {
  // TODO
}
// insert(5)
template <typename K, typename V, typename H, typename EQ, typename C>
template <typename IT>
void kvpq<K, V, H, EQ, C>::insert(IT first, IT last) {
  // TODO
}

// insert_or_assign(1)
template <typename K, typename V, typename H, typename EQ, typename C>
template <typename M>
std::pair<kvpq_iterator<kvpq<K, V, H, EQ, C>>, bool>
kvpq<K, V, H, EQ, C>::insert_or_assign(const K &k, M &&v) {
  // TODO
}
// insert_or_assign(2)
template <typename K, typename V, typename H, typename EQ, typename C>
template <typename M>
std::pair<kvpq_iterator<kvpq<K, V, H, EQ, C>>, bool>
kvpq<K, V, H, EQ, C>::insert_or_assign(K &&k, M &&v) {
  // TODO
}

template <typename K, typename V, typename H, typename EQ, typename C>
template <typename... ARGS>
std::pair<kvpq_iterator<kvpq<K, V, H, EQ, C>>, bool>
kvpq<K, V, H, EQ, C>::emplace(ARGS &&... args) {
  // TODO
}

// try_emplace(1)
template <typename K, typename V, typename H, typename EQ, typename C>
template <typename... ARGS>
std::pair<kvpq_iterator<kvpq<K, V, H, EQ, C>>, bool>
kvpq<K, V, H, EQ, C>::try_emplace(const K &k, ARGS &&... args) {
  // TODO
}
// try_emplace(2)
template <typename K, typename V, typename H, typename EQ, typename C>
template <typename... ARGS>
std::pair<kvpq_iterator<kvpq<K, V, H, EQ, C>>, bool>
kvpq<K, V, H, EQ, C>::try_emplace(K &&k, ARGS &&... args) {
  // TODO
}

template <typename K, typename V, typename H, typename EQ, typename C>
kvpq_iterator<kvpq<K, V, H, EQ, C>>
kvpq<K, V, H, EQ, C>::erase(kvpq_const_iterator<kvpq<K, V, H, EQ, C>> pos) {
  // TODO
}
template <typename K, typename V, typename H, typename EQ, typename C>
kvpq_iterator<kvpq<K, V, H, EQ, C>>
kvpq<K, V, H, EQ, C>::erase(kvpq_const_iterator<kvpq<K, V, H, EQ, C>> first,
                            kvpq_const_iterator<kvpq<K, V, H, EQ, C>> last) {
  // TODO
}
template <typename K, typename V, typename H, typename EQ, typename C>
typename kvpq<K, V, H, EQ, C>::size_type
kvpq<K, V, H, EQ, C>::erase(const K &k) {
  // TODO
}

template <typename K, typename V, typename H, typename EQ, typename C>
void kvpq<K, V, H, EQ, C>::swap(kvpq &o) {
  // TODO
}

// merge(1)
template <typename K, typename V, typename H, typename EQ, typename C>
template <typename H2, typename P2, typename C2>
void kvpq<K, V, H, EQ, C>::merge(const kvpq<K, V, H2, P2, C2> &o) {
  // TODO
}
// merge(2)
template <typename K, typename V, typename H, typename EQ, typename C>
template <typename H2, typename P2, typename C2>
void kvpq<K, V, H, EQ, C>::merge(kvpq<K, V, H2, P2, C2> &&o) {
  // TODO
}

// Lookup
template <typename K, typename V, typename H, typename EQ, typename C>
const std::pair<K, V> &kvpq<K, V, H, EQ, C>::top() const {
  // TODO
}

template <typename K, typename V, typename H, typename EQ, typename C>
V &kvpq<K, V, H, EQ, C>::at(const K &k) {
  // TODO
}
template <typename K, typename V, typename H, typename EQ, typename C>
const V &kvpq<K, V, H, EQ, C>::at(const K &k) const {
  // TODO
}

template <typename K, typename V, typename H, typename EQ, typename C>
V &kvpq<K, V, H, EQ, C>::operator[](const K &k) {
  // TODO
}
template <typename K, typename V, typename H, typename EQ, typename C>
V &kvpq<K, V, H, EQ, C>::operator[](K &&k) {
  // TODO
}

template <typename K, typename V, typename H, typename EQ, typename C>
kvpq_iterator<kvpq<K, V, H, EQ, C>> kvpq<K, V, H, EQ, C>::find(const K &k) {
  // TODO
}
template <typename K, typename V, typename H, typename EQ, typename C>
kvpq_const_iterator<kvpq<K, V, H, EQ, C>>
kvpq<K, V, H, EQ, C>::find(const K &k) const {
  // TODO
}
template <typename K, typename V, typename H, typename EQ, typename C>
bool kvpq<K, V, H, EQ, C>::contains(const K &k) const {
  // TODO
}

// Hash policy
template <typename K, typename V, typename H, typename EQ, typename C>
float kvpq<K, V, H, EQ, C>::load_factor() const {
  // TODO
}
template <typename K, typename V, typename H, typename EQ, typename C>
float kvpq<K, V, H, EQ, C>::max_load_factor() const {
  // TODO
}
template <typename K, typename V, typename H, typename EQ, typename C>
void kvpq<K, V, H, EQ, C>::max_load_factor(float lf) {
  // TODO
}
template <typename K, typename V, typename H, typename EQ, typename C>
void kvpq<K, V, H, EQ, C>::rehash(size_type bucket_count) {
  // TODO
}
template <typename K, typename V, typename H, typename EQ, typename C>
void kvpq<K, V, H, EQ, C>::reserve(size_type count) {
  // TODO
}

// Non-member functions
template <typename K, typename V, typename H, typename EQ, typename C>
bool operator==(const kvpq<K, V, H, EQ, C> &lhs,
                const kvpq<K, V, H, EQ, C> &rhs) {
  // TODO
}

} // namespace ds
