// A combination of an unordered map and a priority queue
#pragma once

#include <functional>       // equal_to, hash, less
#include <initializer_list> // initializer_list
#include <memory>           // unique_ptr
#include <utility>          // move, pair
#include <variant>          // monostate

#include "kvpq_fwd.hpp"

#include "../intrusive/pair.hpp" // pair

namespace ds {
using std::forward;
using std::move;
using std::ptrdiff_t;
using std::size_t;

template <typename KVPQ> struct kvpq_const_iterator {
  using ci = kvpq_const_iterator;
  using difference_type = ptrdiff_t;
  using value_type = typename KVPQ::value_type;
  using pointer = value_type*;
  using reference = value_type&;
  using iterator_category = std::random_access_iterator_tag;

  kvpq_const_iterator() = delete;
  kvpq_const_iterator(const typename KVPQ::heap_type* elt) : _elt(elt) {}

  bool operator==(const ci& o) const { return _elt == o._elt; }
  bool operator!=(const ci& o) const { return _elt != o._elt; }
  bool operator<(const ci& o) const { return _elt < o._elt; }
  bool operator<=(const ci& o) const { return _elt <= o._elt; }
  bool operator>(const ci& o) const { return _elt > o._elt; }
  bool operator>=(const ci& o) const { return _elt >= o._elt; }

  const value_type& operator*() const { return _elt->other()->get(); }
  const typename KVPQ::table_type& operator->() const { return *_elt->other(); }
  const value_type& operator[](size_t n) const { return *(*this + n); }

  ci operator++() { return ci(++_elt); }
  ci operator++(int) { return ci(_elt++); }
  ci operator--() { return ci(--_elt); }
  ci operator--(int) { return ci(_elt--); }
  ci operator+=(ptrdiff_t n) { return ci(_elt += n); }
  ci operator-=(ptrdiff_t n) { return ci(_elt -= n); }
  ci operator+(ptrdiff_t n) const { return ci(_elt + n); }
  friend ci operator+(ptrdiff_t n, const ci& it) { return it + n; }
  ci operator-(ptrdiff_t n) const { return ci(_elt - n); }
  ptrdiff_t operator-(const ci& it) const { return _elt - it._elt; }

 protected:
  const typename KVPQ::heap_type* _elt;
};

template <typename KVPQ> struct kvpq_iterator : kvpq_const_iterator<KVPQ> {
  using i = kvpq_iterator;
  using ci = kvpq_const_iterator<KVPQ>;

  kvpq_iterator(typename KVPQ::heap_type* elt) : ci(elt) {}
  operator ci&() { return ci(*this); }
  operator const ci&() const { return ci(*this); }

  typename KVPQ::value_type& operator*() { return this->_elt->other()->get(); }
  typename KVPQ::table_type& operator->() { return this->_elt->other(); }
  typename KVPQ::value_type& operator[](size_t n) { return *(*this + n); }

  i operator++() { return ++ci(*this); }
  i operator++(int) { return ci(*this)++; }
  i operator--() { return --ci(*this); }
  i operator--(int) { return ci(*this)--; }
  i operator+=(ptrdiff_t n) { return ci(*this) += n; }
  i operator-=(ptrdiff_t n) { return ci(*this) -= n; }
  i operator+(ptrdiff_t n) const { return ci(*this) + n; }
  friend i operator+(ptrdiff_t n, const i& it) { return it + n; }
  i operator-(ptrdiff_t n) const { return ci(*this) - n; }

 private:
  kvpq_iterator(ci&& o) : ci(o) {}
}; // namespace ds

template <typename K, typename V, typename H, typename EQ, typename C>
class kvpq {
  using table_type = intrusive::pair<std::pair<K, V>, std::monostate>;
  using heap_type = intrusive::pair<std::monostate, std::pair<K, V>>;

 public:
  using key_type = K;
  using value_compare = C;
  using value_type = std::pair<K, V>;
  using mapped_type = V;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using hasher = H;
  using key_equal = K;
  using reference = value_type&;
  using const_reference = const value_type&;
  using pointer = value_type*;
  using const_pointer = const value_type*;
  using iterator = kvpq_iterator<kvpq>;
  using const_iterator = kvpq_const_iterator<kvpq>;
  friend iterator;
  friend const_iterator;
  inline static constexpr size_t DEFAULT_SIZE = 10;
  // (1)
  kvpq() : kvpq(DEFAULT_SIZE) {}
  explicit kvpq(size_t bucket_count, const H& = H(), const EQ& = EQ(),
                const C& = C());
  // (2)
  template <typename IT>
  kvpq(IT b, IT e, size_t bucket_count = DEFAULT_SIZE, const H& = H(),
       const EQ& = EQ(), const C& = C());

  // (3)
  kvpq(const kvpq&);

  // (4)
  kvpq(kvpq&&);

  // (5)
  explicit kvpq(std::initializer_list<std::pair<K, V>> init,
                size_t bucket_count = DEFAULT_SIZE, const H& hash = H(),
                const EQ& key_equal = EQ(), const C& comp = C())
      : kvpq(init.begin(), init.end(), bucket_count, hash, key_equal, comp) {}

  ~kvpq();

  kvpq& operator=(const kvpq&);
  kvpq& operator=(kvpq&&);

  // Iterators
  iterator begin() noexcept { return _heap; }
  const_iterator begin() const noexcept { return _heap; }
  const_iterator cbegin() const noexcept { return begin(); }
  iterator end() noexcept { return _heap + _size; }
  const_iterator end() const noexcept { return _heap + _size; }
  const_iterator cend() const noexcept { return end(); }

  // Capacity
  [[nodiscard]] bool empty() const noexcept { return _size == 0; }
  size_t size() const noexcept { return _size; }
  size_t capacity() const noexcept { return _bucket_mask + 1; }
  size_t max_size() const noexcept { return -1; }

  // Modifiers
  void push(const std::pair<K, V>& p) { insert(p); }
  void push(std::pair<K, V>&& p) { insert(p); }
  void pop() { erase(begin()); }
  void clear() noexcept;

  // insert(1)
  std::pair<iterator, bool> insert(const std::pair<K, V>&);
  std::pair<iterator, bool> insert(std::pair<K, V>&&);
  // insert(2)
  template <typename P> std::pair<iterator, bool> insert(P&& p) {
    return emplace(forward(p));
  }
  // insert(3)
  iterator insert(const_iterator /* hint */, const std::pair<K, V>& p) {
    return insert(p);
  }
  iterator insert(const_iterator /* hint */, std::pair<K, V>&& p) {
    return insert(move(p));
  }
  // insert(4)
  template <typename P>
  std::pair<iterator, bool> insert(const_iterator /* hint */, P&& p) {
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
  std::pair<iterator, bool> insert_or_assign(const K&, M&&);
  // insert_or_assign(2)
  template <typename M> std::pair<iterator, bool> insert_or_assign(K&&, M&&);
  // insert_or_assign(3)
  template <typename M>
  std::pair<iterator, bool> insert_or_assign(const_iterator /* hint */,
                                             const K& k, M&& v) {
    return insert_or_assign(k, forward(v));
  }
  // insert_or_assign(4)
  template <typename M>
  std::pair<iterator, bool> insert_or_assign(const_iterator /* hint */, K&& k,
                                             M&& v) {
    return insert_or_assign(move(k), forward(v));
  }

  template <typename... ARGS> std::pair<iterator, bool> emplace(ARGS&&...);
  template <typename... ARGS>
  iterator emplace_hint(const_iterator /* hint */, ARGS&&... args) {
    return emplace(forward(args)...).first;
  }

  // try_emplace(1)
  template <typename... ARGS>
  std::pair<iterator, bool> try_emplace(const K&, ARGS&&...);
  // try_emplace(2)
  template <typename... ARGS>
  std::pair<iterator, bool> try_emplace(K&&, ARGS&&...);
  // try_emplace(3)
  template <typename... ARGS>
  std::pair<iterator, bool> try_emplace(const_iterator /* hint */, const K& k,
                                        ARGS&&... args) {
    return try_emplace(k, forward(args)...);
  }
  // try_emplace(4)
  template <typename... ARGS>
  std::pair<iterator, bool> try_emplace(const_iterator /* hint */, K&& k,
                                        ARGS&&... args) {
    return try_emplace(move(k), forward(args)...);
  }

  iterator erase(const_iterator pos);
  size_t erase(const K&);
  void swap(kvpq&);

  // merge(1)
  template <typename H2, typename P2, typename C2>
  void merge(const kvpq<K, V, H2, P2, C2>&);
  // merge(2)
  template <typename H2, typename P2, typename C2>
  void merge(kvpq<K, V, H2, P2, C2>&&);

  // Lookup
  std::pair<K, V>& top() { return *begin(); }
  const std::pair<K, V>& top() const { return *begin(); }
  V& at(const K&);
  const V& at(const K&) const;
  V& operator[](const K&);
  V& operator[](K&&);
  size_t count(const K& k) const { return contains(k); }

  iterator find(const K&);
  const_iterator find(const K&) const;
  bool contains(const K&) const;

  // Hash policy
  float load_factor() const;
  float max_load_factor() const;
  void max_load_factor(float lf);
  void rehash(size_t bucket_count);
  void reserve(size_t count);

  // Observers
  H hash_function() const { return _hash; }
  EQ key_eq() const { return _key_equal; }
  C comp_function() const { return _comp; }

  // Non-member functionspa
  bool operator==(const kvpq& o);
  bool operator!=(const kvpq& o) { return !(*this == o); }
  friend void swap(const kvpq& lhs, const kvpq& rhs) { return lhs.swap(rhs); }

 private:
  [[nodiscard]] inline size_t next(size_t i) { return (i + 1) & _bucket_mask; }
  [[nodiscard]] inline bool free(size_t i) { return !_offset[i]; }
  inline size_t hash_at(size_t i) { return _offset[i] + i + 1; }
  inline void set_hash_at(size_t i, size_t h) { _offset[i] = h - i - 1; }
  inline void clear_hash_at(size_t i) { set_hash_at(i, next(i)); }
  [[nodiscard]] static inline size_t parent(size_t i) {
    return ((i + 1) >> 1) - 1;
  }
  [[nodiscard]] static inline size_t lchild(size_t i) {
    return ((i + 1) << 1) - 1;
  }
  [[nodiscard]] static inline size_t rchild(size_t i) { return (i + 1) << 1; }

  struct Mask {
    Mask(size_t i) : _i(size_t(-1) >> __builtin_clzl(i)) {}
    Mask& operator=(size_t i) {
      _i = size_t(-1) >> __builtin_clzl(i);
      return *this;
    }
    operator size_t&() { return _i; }
    operator const size_t&() const { return _i; }
    size_t _i;
  } _bucket_mask;
  [[no_unique_address]] H _hash;
  [[no_unique_address]] EQ _key_equal;
  [[no_unique_address]] C _comp;
  size_t _size = 0;
  size_t* _offset;
  table_type* _table;
  heap_type* _heap;
};

// (1)
template <typename K, typename V, typename H, typename EQ, typename C>
kvpq<K, V, H, EQ, C>::kvpq(size_t bucket_count, const H& hash,
                           const EQ& key_equal, const C& comp)
    : _bucket_mask(bucket_count - 1), _hash(hash), _key_equal(key_equal),
      _comp(comp) {
  _offset = new size_t[capacity()];
  _table = (table_type*)operator new[](capacity() * sizeof(table_type));
  _heap = (heap_type*)operator new[](capacity() * sizeof(heap_type));
}
// (2)
template <typename K, typename V, typename H, typename EQ, typename C>
template <typename IT>
kvpq<K, V, H, EQ, C>::kvpq(IT b, IT e, size_t bucket_count, const H& hash,
                           const EQ& key_equal, const C& comp)
    : kvpq(bucket_count, hash, key_equal, comp) {
  insert(b, e);
}
// (3)
template <typename K, typename V, typename H, typename EQ, typename C>
kvpq<K, V, H, EQ, C>::kvpq(const kvpq& o)
    : kvpq(o._bucket_mask + 1, o._hash, o._key_equal, o._comp) {
  for (size_t i = 0; i < o.size(); ++i) {
    ++_size;
    size_t j = o._heap[i].other() - o._table;
    _offset[j] = o._offset[j];
    auto [table_entry, heap_entry] = table_type::make(o._table[j].get());
    new (_table + j) table_type(move(table_entry));
    new (_heap + i) heap_type(move(heap_entry));
  }
}

// (4)
template <typename K, typename V, typename H, typename EQ, typename C>
kvpq<K, V, H, EQ, C>::kvpq(kvpq&& o)
    : _bucket_mask(o._bucket_mask), _hash(move(o._hash)),
      _key_equal(move(o._key_equal)), _comp(move(o._comp)), _size(o._size),
      _offset(o._offset), _table(o._table), _heap(o._heap) {
  o._size = 0;
  o._offset = nullptr;
  o._table = nullptr;
  o._heap = nullptr;
}

template <typename K, typename V, typename H, typename EQ, typename C>
kvpq<K, V, H, EQ, C>::~kvpq() {
  clear();
  delete[] _offset;
  operator delete[](_table);
  operator delete[](_heap);
}

template <typename K, typename V, typename H, typename EQ, typename C>
kvpq<K, V, H, EQ, C>& kvpq<K, V, H, EQ, C>::operator=(const kvpq& o) {
  if (_bucket_mask != o._bucket_mask) {
    ~kvpq();
    return *(new (this) kvpq(o));
  }

  clear();

  for (size_t i = 0; i < o.size(); ++i) {
    ++_size;
    size_t j = o._heap[i].other() - o._table;
    _offset[j] = o._offset[j];
    auto [table_entry, heap_entry] = table_type::make(o._table[j].get());
    new (_table + j) table_type(move(table_entry));
    new (_heap + i) heap_type(move(heap_entry));
  }

  return *this;
}

template <typename K, typename V, typename H, typename EQ, typename C>
kvpq<K, V, H, EQ, C>& kvpq<K, V, H, EQ, C>::operator=(kvpq&& o) {
  ~kvpq();
  return *(new (this) kvpq(move(o)));
}

// Modifiers
template <typename K, typename V, typename H, typename EQ, typename C>
void kvpq<K, V, H, EQ, C>::clear() noexcept {
  for (size_t i = 0; i < size(); ++i) {
    size_t j = _heap[i].other() - _table;
    _table[j].~table_type();
    _heap[i].~heap_type();
    clear_hash_at(j);
    --_size;
  }
}

// insert(1)
template <typename K, typename V, typename H, typename EQ, typename C>
std::pair<kvpq_iterator<kvpq<K, V, H, EQ, C>>, bool>
kvpq<K, V, H, EQ, C>::insert(const std::pair<K, V>& p) {
  return emplace(p);
}
template <typename K, typename V, typename H, typename EQ, typename C>
std::pair<kvpq_iterator<kvpq<K, V, H, EQ, C>>, bool>
kvpq<K, V, H, EQ, C>::insert(std::pair<K, V>&& p) {
  return emplace(move(p));
}
// insert(5)
template <typename K, typename V, typename H, typename EQ, typename C>
template <typename IT>
void kvpq<K, V, H, EQ, C>::insert(IT first, IT last) {
  while (first != last) { insert(*first); }
}

// insert_or_assign(1)
template <typename K, typename V, typename H, typename EQ, typename C>
template <typename M>
std::pair<kvpq_iterator<kvpq<K, V, H, EQ, C>>, bool>
kvpq<K, V, H, EQ, C>::insert_or_assign(const K& k, M&& v) {
  if (auto it = find(k); it == end()) {
    return emplace(std::piecewise_construct, k, forward(v));
  } else {
    it->second = forward(v);
    return {it, false};
  }
}
// insert_or_assign(2)
template <typename K, typename V, typename H, typename EQ, typename C>
template <typename M>
std::pair<kvpq_iterator<kvpq<K, V, H, EQ, C>>, bool>
kvpq<K, V, H, EQ, C>::insert_or_assign(K&& k, M&& v) {
  if (auto it = find(k); it == end()) {
    return emplace(std::piecewise_construct, move(k), forward(v));
  } else {
    it->second = forward(v);
    return {it, false};
  }
}

template <typename K, typename V, typename H, typename EQ, typename C>
template <typename... ARGS>
std::pair<kvpq_iterator<kvpq<K, V, H, EQ, C>>, bool>
kvpq<K, V, H, EQ, C>::emplace(ARGS&&... args) {
  auto [table_entry, heap_entry] = table_type::make(forward(args)...);
  reserve(_size + 1);
  size_t h = _hash(table_entry->first), i = h;
  while (!free(i)) {
    if (hash_at(i) == h && _key_eq(_table[i].first, table_entry.first)) {
      return {_table[i].other(), false};
    }
    i = next(i);
  }

  set_hash_at(i, h);
  new (_table + i) table_type(move(table_entry));
  size_t j = _size;
  while (j && _comp(_heap[parent(j)].other()->first, table_entry.first)) {
    if (j == _size) {
      new (_heap + j) heap_type(move(_heap[parent(j)]));
    } else {
      _heap[j] = move(_heap[parent(j)]);
    }
    j = parent(j);
  }
  if (j == _size) {
    new (_heap + j) heap_type(move(heap_entry));
  } else {
    _heap[j] = move(heap_entry);
  }
  ++_size;
  return {_heap + j, true};
}

// try_emplace(1)
template <typename K, typename V, typename H, typename EQ, typename C>
template <typename... ARGS>
std::pair<kvpq_iterator<kvpq<K, V, H, EQ, C>>, bool>
kvpq<K, V, H, EQ, C>::try_emplace(const K& k, ARGS&&... args) {
  return emplace(std::make_pair(k, V(std::forward(args)...)));
}
// try_emplace(2)
template <typename K, typename V, typename H, typename EQ, typename C>
template <typename... ARGS>
std::pair<kvpq_iterator<kvpq<K, V, H, EQ, C>>, bool>
kvpq<K, V, H, EQ, C>::try_emplace(K&& k, ARGS&&... args) {
  return emplace(std::make_pair(move(k), V(std::forward(args)...)));
}

template <typename K, typename V, typename H, typename EQ, typename C>
kvpq_iterator<kvpq<K, V, H, EQ, C>>
kvpq<K, V, H, EQ, C>::erase(kvpq_const_iterator<kvpq<K, V, H, EQ, C>> pos) {
  const table_type& table_entry = *pos;
  size_t j = table_entry.other() - _heap;
  while (j) {
    _heap[j] = move(_heap[parent(j)]);
    j = parent(j);
  }
  while (lchild(j) < _size) {
    if (rchild(j) < _size && _comp(lchild(j), rchild(j))) {
      _heap[j] = move(_heap[rchild(j)]);
      j = rchild(j);
    } else {
      _heap[j] = move(_heap[lchild(j)]);
      j = lchild(j);
    }
  }
  _heap[j].~heap_type();

  size_t i = &table_entry - _table;
  for (size_t j = i; !free(j); j = next(j)) {
    if (((hash_at(j) - i) & _bucket_mask) < ((hash_at(i) - i) & _bucket_mask)) {
      _table[i] = move(_table[j]);
      set_hash_at(i, hash_at(j));
      i = j;
    }
  }
  _table[i].~table_type();
}
template <typename K, typename V, typename H, typename EQ, typename C>
size_t kvpq<K, V, H, EQ, C>::erase(const K& k) {
  if (auto it = find(k); it == end()) {
    return 0;
  } else {
    erase(it);
    return 1;
  }
}

template <typename K, typename V, typename H, typename EQ, typename C>
void kvpq<K, V, H, EQ, C>::swap(kvpq& o) {
  using std::swap;
  swap(_bucket_mask, o._bucket_mask);
  swap(_hash, o._hash);
  swap(_key_equal, o._key_equal);
  swap(_comp, o._comp);
  swap(_size, o._size);
  swap(_offset, o._offset);
  swap(_table, o._table);
  swap(_heap, o._heap);
}

// merge(1)
template <typename K, typename V, typename H, typename EQ, typename C>
template <typename H2, typename P2, typename C2>
void kvpq<K, V, H, EQ, C>::merge(const kvpq<K, V, H2, P2, C2>& o) {
  insert(o.begin(), o.end());
}
// merge(2)
template <typename K, typename V, typename H, typename EQ, typename C>
template <typename H2, typename P2, typename C2>
void kvpq<K, V, H, EQ, C>::merge(kvpq<K, V, H2, P2, C2>&& o) {
  for (std::pair<K, V>& elt : o) { insert(move(elt)); }
  o.clear();
}

// Lookup
template <typename K, typename V, typename H, typename EQ, typename C>
V& kvpq<K, V, H, EQ, C>::at(const K& k) {
  if (auto it = find(k); it == end()) {
    throw std::out_of_range("V& kvpq::at(const K&)");
  } else {
    return it->second;
  }
}
template <typename K, typename V, typename H, typename EQ, typename C>
const V& kvpq<K, V, H, EQ, C>::at(const K& k) const {
  if (auto it = find(k); it == end()) {
    throw std::out_of_range("const V& kvpq::at(const K&) const");
  } else {
    return it->second;
  }
}

template <typename K, typename V, typename H, typename EQ, typename C>
V& kvpq<K, V, H, EQ, C>::operator[](const K& k) {
  return find(k)->second;
}
template <typename K, typename V, typename H, typename EQ, typename C>
V& kvpq<K, V, H, EQ, C>::operator[](K&& k) {
  return find(k)->second;
}

template <typename K, typename V, typename H, typename EQ, typename C>
kvpq_iterator<kvpq<K, V, H, EQ, C>> kvpq<K, V, H, EQ, C>::find(const K& k) {
  return const_cast<const kvpq&>(*this).find(k);
}
template <typename K, typename V, typename H, typename EQ, typename C>
kvpq_const_iterator<kvpq<K, V, H, EQ, C>>
kvpq<K, V, H, EQ, C>::find(const K& k) const {
  size_t h = _hash(k);
  for (size_t i = h; !free(i); i = next(i)) {
    if (hash_at(i) == h && _key_eq(_table[i].first, k)) {
      return _table[i].other();
    }
  }
  return end();
}
template <typename K, typename V, typename H, typename EQ, typename C>
bool kvpq<K, V, H, EQ, C>::contains(const K& k) const {
  return find(k) == end();
}

// Hash policy
template <typename K, typename V, typename H, typename EQ, typename C>
float kvpq<K, V, H, EQ, C>::load_factor() const {
  // TODO
  return 0;
}
template <typename K, typename V, typename H, typename EQ, typename C>
float kvpq<K, V, H, EQ, C>::max_load_factor() const {
  // TODO
  return 0;
}
template <typename K, typename V, typename H, typename EQ, typename C>
void kvpq<K, V, H, EQ, C>::max_load_factor(float lf) {
  // TODO
}
template <typename K, typename V, typename H, typename EQ, typename C>
void kvpq<K, V, H, EQ, C>::rehash(size_t bucket_count) {
  // TODO
}
template <typename K, typename V, typename H, typename EQ, typename C>
void kvpq<K, V, H, EQ, C>::reserve(size_t count) {
  // TODO
}

// Non-member functions
template <typename K, typename V, typename H, typename EQ, typename C>
bool kvpq<K, V, H, EQ, C>::operator==(const kvpq<K, V, H, EQ, C>& o) {
  if (this == &o) { return true; }
  if (_hash != o._hash || _key_equal != o._key_equal || _comp != o._comp ||
      _size != o._size) {
    return false;
  }
  for (auto it = o.begin(); it != o.end(); ++o) {
    if (auto item = find(it->first);
        item == end() || !_key_eq(item->second, it->second)) {
      return false;
    }
  }
  return true;
}
} // namespace ds
