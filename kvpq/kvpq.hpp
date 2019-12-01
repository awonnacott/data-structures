// A combination of an unordered map and a priority queue
#pragma once

#include <cmath>            // ceil, pow, sqrt
#include <cstddef>          // ptrdiff_t, size_t
#include <functional>       // equal_to, hash, less
#include <initializer_list> // initializer_list
#include <iterator>         // iterator_traits, random_access_iterator_tag
#include <memory>           // unique_ptr
#include <stdexcept>        // out_of_range
#include <type_traits>      // is_base_of_v, remove_const_t
#include <utility> // forward, make_pair, move, pair, piecewise_construct, swap
#include <variant> // monostate

#include "../intrusive/pair.hpp" // pair

#include "kvpq_fwd.hpp"

namespace ds {
using std::forward;
using std::move;

template <typename KVPQ> struct kvpq_const_iterator {
  using ci = kvpq_const_iterator;
  using size_type = typename KVPQ::size_type;
  using difference_type = typename KVPQ::difference_type;
  using value_type = const typename KVPQ::value_type;
  using pointer = value_type*;
  using reference = value_type&;
  using iterator_category = std::random_access_iterator_tag;

  kvpq_const_iterator() = delete;
  explicit kvpq_const_iterator(const typename KVPQ::heap_type* elt)
      : elt_(elt) {}

  bool operator==(const ci& o) const { return elt_ == o.elt_; }
  bool operator!=(const ci& o) const { return elt_ != o.elt_; }
  bool operator<(const ci& o) const { return elt_ < o.elt_; }
  bool operator<=(const ci& o) const { return elt_ <= o.elt_; }
  bool operator>(const ci& o) const { return elt_ > o.elt_; }
  bool operator>=(const ci& o) const { return elt_ >= o.elt_; }

  const value_type& operator*() const { return elt_->other()->get(); }
  const typename KVPQ::table_type& operator->() const { return *elt_->other(); }
  const value_type& operator[](size_type n) const { return *(*this + n); }

  ci& operator++() {
    ++elt_;
    return *this;
  }
  ci& operator++(int) {
    elt_++;
    return *this;
  }
  ci& operator--() {
    --elt_;
    return *this;
  }
  ci& operator--(int) {
    elt_--;
    return *this;
  }
  ci& operator+=(difference_type n) {
    elt_ += n;
    return *this;
  }
  ci& operator-=(difference_type n) {
    elt_ -= n;
    return *this;
  }
  ci operator+(difference_type n) const { return ci(elt_ + n); }
  friend ci operator+(difference_type n, const ci& it) { return it + n; }
  ci operator-(difference_type n) const { return ci(elt_ - n); }
  difference_type operator-(const ci& it) const { return elt_ - it.elt_; }

 protected:
  const typename KVPQ::heap_type* elt_;
};

template <typename KVPQ> struct kvpq_iterator : kvpq_const_iterator<KVPQ> {
  using i = kvpq_iterator;
  using const_iterator = kvpq_const_iterator<KVPQ>;
  using size_type = typename const_iterator::size_type;
  using difference_type = typename const_iterator::difference_type;
  using value_type = std::remove_const_t<typename const_iterator::value_type>;
  using pointer = value_type*;
  using reference = value_type&;
  using iterator_category = std::random_access_iterator_tag;

  explicit kvpq_iterator(typename KVPQ::heap_type* elt) : const_iterator(elt) {}
  operator const_iterator&() { return *this; }
  operator const const_iterator&() const { return *this; }

  typename KVPQ::value_type& operator*() { return this->elt_->other()->get(); }
  typename KVPQ::table_type& operator->() { return this->elt_->other(); }
  typename KVPQ::value_type& operator[](size_type n) { return *(*this + n); }

  i& operator++() {
    ++ci();
    return *this;
  }
  i& operator++(int) {
    ci()++;
    return *this;
  }
  i& operator--() {
    --ci();
    return *this;
  }
  i& operator--(int) {
    ci()--;
    return *this;
  }
  i& operator+=(difference_type n) {
    ci() += n;
    return *this;
  }
  i& operator-=(difference_type n) {
    ci() -= n;
    return *this;
  }
  i operator+(difference_type n) const { return i(ci() + n); }
  friend i operator+(difference_type n, const i& it) { return it + n; }
  i operator-(difference_type n) const { return i(ci() - n); }
  difference_type operator-(const const_iterator& it) const {
    return ci() - it.ci();
  }

 private:
  const_iterator& ci() { return static_cast<const_iterator&>(*this); }
  const const_iterator& ci() const {
    return static_cast<const const_iterator&>(*this);
  }
  explicit kvpq_iterator(const_iterator&& o) : const_iterator(o) {}
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
  using size_type = std::size_t;
  using difference_type = std::ptrdiff_t;
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
  inline static constexpr size_type DEFAULT_BUCKET_COUNT = 10;
  inline static constexpr float DEFAULT_MAX_LOAD_FACTOR = 1.0;
  // (1)
  kvpq() : kvpq(DEFAULT_BUCKET_COUNT) {}
  explicit kvpq(size_type bucket_count, const H& = H(), const EQ& = EQ(),
                const C& = C());
  // (2)
  template <typename IT>
  kvpq(IT b, IT e, size_type bucket_count = DEFAULT_BUCKET_COUNT,
       const H& hash = H(), const EQ& key_equal = EQ(), const C& comp = C())
      : kvpq(bucket_count, hash, key_equal, comp) {
    if constexpr (std::is_base_of_v<
                      std::random_access_iterator_tag,
                      std::iterator_traits<IT>::iterator_category>) {
      // TODO: do not allocate for bucket_count first in this case
      reserve(e - b);
    }
    insert(b, e);
  }

  // (3)
  kvpq(const kvpq&);

  // (4)
  kvpq(kvpq&&);

  // (5)
  explicit kvpq(std::initializer_list<std::pair<K, V>> init,
                size_type bucket_count = DEFAULT_BUCKET_COUNT,
                const H& hash = H(), const EQ& key_equal = EQ(),
                const C& comp = C())
      : kvpq(std::max(bucket_count, size_t(get_bucket_mask(
                                        init.size(), DEFAULT_MAX_LOAD_FACTOR)) +
                                        1),
             init.size, hash, key_equal, comp) {
    insert(init);
  }

  ~kvpq();

  kvpq& operator=(const kvpq&);
  kvpq& operator=(kvpq&& o) {
    ~kvpq();
    return *(new (this) kvpq(move(o)));
  }

  // Iterators
  iterator begin() noexcept { return iterator(heap_); }
  const_iterator begin() const noexcept { return const_iterator(heap_); }
  const_iterator cbegin() const noexcept { return begin(); }
  iterator end() noexcept { return iterator(heap_ + size_); }
  const_iterator end() const noexcept { return const_iterator(heap_ + size_); }
  const_iterator cend() const noexcept { return end(); }

  // Modifiers
  void push(const std::pair<K, V>& p) { insert(p); }
  void push(std::pair<K, V>&& p) { insert(p); }
  void pop() { erase(begin()); }
  void clear() noexcept;

  // insert(1)
  std::pair<iterator, bool> insert(const std::pair<K, V>& p) {
    return emplace(p);
  }
  std::pair<iterator, bool> insert(std::pair<K, V>&& p) {
    return emplace(move(p));
  }
  // insert(2)
  template <typename P> std::pair<iterator, bool> insert(P&& p) {
    return emplace(forward(p));
  }
  // insert(3)
  iterator insert(const_iterator /* hint */, const std::pair<K, V>& p) {
    return insert(p).first;
  }
  iterator insert(const_iterator /* hint */, std::pair<K, V>&& p) {
    return insert(move(p)).first;
  }
  // insert(4)
  template <typename P>
  std::pair<iterator, bool> insert(const_iterator /* hint */, P&& p) {
    return insert(forward(p)).first;
  }
  // insert(5)
  template <typename IT> void insert(IT b, IT e) {
    if constexpr (std::is_base_of_v<
                      std::random_access_iterator_tag,
                      std::iterator_traits<IT>::iterator_category>) {
      reserve(size_ + (e - b));
    }
    while (b != e) { insert(*b++); }
  }
  // insert(6)
  void insert(std::initializer_list<std::pair<K, V>> init) {
    reserve(size_ + init.size());
    insert(init.begin(), init.end());
  }

  // insert_or_assign(1)
  template <typename M>
  std::pair<iterator, bool> insert_or_assign(const K&, M&&);
  // insert_or_assign(2)
  template <typename M> std::pair<iterator, bool> insert_or_assign(K&&, M&&);
  // insert_or_assign(3)
  template <typename M>
  iterator insert_or_assign(const_iterator /* hint */, const K& k, M&& v) {
    return insert_or_assign(k, forward(v)).first;
  }
  // insert_or_assign(4)
  template <typename M>
  iterator insert_or_assign(const_iterator /* hint */, K&& k, M&& v) {
    return insert_or_assign(move(k), forward(v)).first;
  }

  template <typename... ARGS> std::pair<iterator, bool> emplace(ARGS&&...);
  template <typename... ARGS>
  iterator emplace_hint(const_iterator /* hint */, ARGS&&... args) {
    return emplace(forward(args)...).first;
  }

  // try_emplace(1)
  template <typename... ARGS>
  std::pair<iterator, bool> try_emplace(const K& k, ARGS&&... args) {
    return emplace(std::make_pair(k, V(std::forward(args)...)));
  }
  // try_emplace(2)
  template <typename... ARGS>
  std::pair<iterator, bool> try_emplace(K&& k, ARGS&&... args) {
    // TODO: rewrite with perfect forwarding and rebase constructors off this
    // instead of emplace
    return emplace(std::make_pair(move(k), V(std::forward(args)...)));
  }
  // try_emplace(3)
  template <typename... ARGS>
  iterator try_emplace(const_iterator /* hint */, const K& k, ARGS&&... args) {
    return try_emplace(k, forward(args)...).first;
  }
  // try_emplace(4)
  template <typename... ARGS>
  iterator try_emplace(const_iterator /* hint */, K&& k, ARGS&&... args) {
    return try_emplace(move(k), forward(args)...).first;
  }

  iterator erase(const_iterator pos);
  size_type erase(const K&);
  void swap(kvpq&);

  // merge(1)
  template <typename H2, typename P2, typename C2>
  void merge(const kvpq<K, V, H2, P2, C2>& o) {
    reserve(size_ + o.size());
    insert(o.begin(), o.end());
  }
  // merge(2)
  template <typename H2, typename P2, typename C2>
  void merge(kvpq<K, V, H2, P2, C2>&&);

  // Lookup
  std::pair<K, V>& top() { return *begin(); }
  const std::pair<K, V>& top() const { return *begin(); }
  V& at(const K&);
  const V& at(const K&) const;
  V& operator[](const K& k) { return find(k)->second; }
  V& operator[](K&& k) { return find(k)->second; }
  size_type count(const K& k) const { return contains(k); }
  iterator find(const K& k) { return const_cast<const kvpq&>(*this).find(k); }
  const_iterator find(const K&) const;
  bool contains(const K& k) const { return find(k) != end(); }

  // Capacity
  [[nodiscard]] bool empty() const noexcept { return size_ == 0; }
  size_type size() const noexcept { return size_; }
  size_type capacity() const noexcept { return bucket_mask_ + 1; }
  static constexpr size_type max_size() noexcept { return -1; }
  // std::unordered_map has expected number of probes for an unsuccessful search
  // - 1 = bucket_count * load_factor = size. We define load_factor to be the
  // number of probes required for an unsuccessful search - 1 divided by
  // bucket_count instead of size / bucket_count. This maintains the invariant
  // size / bucket_count < 1 even when load_factor() > 1.
  float load_factor() const { return get_load_factor(size_, bucket_mask_); }
  float max_load_factor() const { return max_load_factor_; }
  void max_load_factor(float lf) {
    resize(get_bucket_mask(size_, lf));
    max_load_factor_ = lf;
    capacity_ = get_capacity(max_load_factor_, bucket_mask_);
  }
  void rehash(size_type bucket_count) { resize(Mask(bucket_count - 1)); }
  void reserve(size_type count) {
    if (count > capacity_) { resize(get_bucket_mask(count, max_load_factor_)); }
  }

  // Observers
  H hash_function() const { return hash_; }
  EQ key_eq() const { return key_equal_; }
  C comp_function() const { return comp_; }

  // Non-member functionspa
  bool operator==(const kvpq& o) const;
  bool operator!=(const kvpq& o) const { return !(*this == o); }
  friend void swap(const kvpq& lhs, const kvpq& rhs) { return lhs.swap(rhs); }

 private:
  [[nodiscard]] inline size_type next(size_type i) const {
    return (i + 1) & bucket_mask_;
  }
  [[nodiscard]] inline bool free(size_type i) const { return !offset_[i]; }
  [[nodiscard]] inline size_type hash_at(size_type i) const {
    return offset_[i] + i + 1;
  }
  inline void set_hash_at(size_type i, size_type h) { offset_[i] = h - i - 1; }
  inline void clear_hash_at(size_type i) { set_hash_at(i, next(i)); }
  [[nodiscard]] static constexpr inline size_type parent(size_type i) {
    return ((i + 1) >> 1) - 1;
  }
  [[nodiscard]] static constexpr inline size_type lchild(size_type i) {
    return ((i + 1) << 1) - 1;
  }
  [[nodiscard]] static constexpr inline size_type rchild(size_type i) {
    return (i + 1) << 1;
  }

  struct Mask {
    static_assert(sizeof(size_type) == sizeof(unsigned long));
    constexpr explicit Mask(size_type i)
        : i_(size_type(-1) >> __builtin_clzl(i)) {}
    constexpr Mask& operator=(size_type i) {
      i_ = size_type(-1) >> __builtin_clzl(i);
      return *this;
    }
    constexpr operator size_type&() { return i_; }
    constexpr operator const size_type&() const { return i_; }
    size_type i_;
  };
  [[nodiscard]] static constexpr inline float
  get_load_factor(size_type size, Mask bucket_mask) {
    return (std::pow(1. /
                         (1. - float(size) / float(size_type(bucket_mask) + 1)),
                     2) -
            1) *
           0.5;
  }
  [[nodiscard]] static constexpr inline Mask
  get_bucket_mask(size_type size, float load_factor) {
    return size
               ? Mask(
                     size_t(std::ceil(
                         size / (1. - 1. / std::sqrt(load_factor * 2. + 1.)))) -
                     1)
               : Mask(1);
  }
  [[nodiscard]] static inline size_type get_capacity(float load_factor,
                                                     Mask bucket_mask) {
    return (1. - 1. / std::sqrt(load_factor * 2. + 1.)) *
           (size_t(bucket_mask) + 1);
  }
  void resize(Mask bucket_mask);

  Mask bucket_mask_;
  [[no_unique_address]] H hash_;
  [[no_unique_address]] EQ key_equal_;
  [[no_unique_address]] C comp_;
  float max_load_factor_ = DEFAULT_MAX_LOAD_FACTOR;
  size_type capacity_;
  size_type size_ = 0;
  size_type* offset_;
  table_type* table_;
  heap_type* heap_;
};

// (1)
template <typename K, typename V, typename H, typename EQ, typename C>
kvpq<K, V, H, EQ, C>::kvpq(size_type bucket_count, const H& hash,
                           const EQ& key_equal, const C& comp)
    : bucket_mask_(bucket_count - 1), hash_(hash), key_equal_(key_equal),
      comp_(comp), capacity_(get_capacity(max_load_factor_, bucket_mask_)) {
  offset_ = new size_type[capacity()];
  table_ = (table_type*)operator new[](capacity() * sizeof(table_type));
  heap_ = (heap_type*)operator new[](capacity() * sizeof(heap_type));
}
// (3)
template <typename K, typename V, typename H, typename EQ, typename C>
kvpq<K, V, H, EQ, C>::kvpq(const kvpq& o)
    : kvpq(o.capacity(), o.hash_, o.key_equal_, o.comp_) {
  max_load_factor_ = o.max_load_factor_;
  capacity_ = o.capacity_;
  for (size_type i = 0; i < o.size(); ++i) {
    ++size_;
    size_type j = o.heap_[i].other() - o.table_;
    offset_[j] = o.offset_[j];
    auto [table_entry, heap_entry] = table_type::make(o.table_[j].get());
    new (table_ + j) table_type(move(table_entry));
    new (heap_ + i) heap_type(move(heap_entry));
  }
}

// (4)
template <typename K, typename V, typename H, typename EQ, typename C>
kvpq<K, V, H, EQ, C>::kvpq(kvpq&& o)
    : bucket_mask_(o.bucket_mask_), hash_(move(o.hash_)),
      key_equal_(move(o.key_equal_)), comp_(move(o.comp_)),
      max_load_factor_(o.max_load_factor_), capacity_(o.capacity_),
      size_(o.size_), offset_(o.offset_), table_(o.table_), heap_(o.heap_) {
  o.size_ = 0;
  o.offset_ = nullptr;
  o.table_ = nullptr;
  o.heap_ = nullptr;
}

template <typename K, typename V, typename H, typename EQ, typename C>
kvpq<K, V, H, EQ, C>::~kvpq() {
  clear();
  delete[] offset_;
  operator delete[](table_);
  operator delete[](heap_);
}

template <typename K, typename V, typename H, typename EQ, typename C>
kvpq<K, V, H, EQ, C>& kvpq<K, V, H, EQ, C>::operator=(const kvpq& o) {
  if (bucket_mask_ != o.bucket_mask_) {
    ~kvpq();
    return *(new (this) kvpq(o));
  }

  clear();
  hash_ = o.hash_;
  key_equal_ = o.key_equal_;
  comp_ = o.comp_;
  max_load_factor_ = o.max_load_factor_;
  capacity_ = o.capacity_;

  for (size_type i = 0; i < o.size(); ++i) {
    ++size_;
    size_type j = o.heap_[i].other() - o.table_;
    offset_[j] = o.offset_[j];
    auto [table_entry, heap_entry] = table_type::make(o.table_[j].get());
    new (table_ + j) table_type(move(table_entry));
    new (heap_ + i) heap_type(move(heap_entry));
  }

  return *this;
}

// Modifiers
template <typename K, typename V, typename H, typename EQ, typename C>
void kvpq<K, V, H, EQ, C>::clear() noexcept {
  for (size_type i = 0; i < size(); ++i) {
    size_type j = heap_[i].other() - table_;
    table_[j].~table_type();
    heap_[i].~heap_type();
    clear_hash_at(j);
    --size_;
  }
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
  reserve(size_ + 1);
  size_type h = hash_(table_entry->first), i = h;
  while (!free(i)) {
    if (hash_at(i) == h && key_eq_(table_[i].first, table_entry.first)) {
      return {table_[i].other(), false};
    }
    i = next(i);
  }

  set_hash_at(i, h);
  new (table_ + i) table_type(move(table_entry));
  size_type j = size_;
  while (j && comp_(heap_[parent(j)].other()->first, table_entry.first)) {
    if (j == size_) {
      new (heap_ + j) heap_type(move(heap_[parent(j)]));
    } else {
      heap_[j] = move(heap_[parent(j)]);
    }
    j = parent(j);
  }
  if (j == size_) {
    new (heap_ + j) heap_type(move(heap_entry));
  } else {
    heap_[j] = move(heap_entry);
  }
  ++size_;
  return {heap_ + j, true};
}
template <typename K, typename V, typename H, typename EQ, typename C>
kvpq_iterator<kvpq<K, V, H, EQ, C>>
kvpq<K, V, H, EQ, C>::erase(const_iterator pos) {
  const table_type& table_entry = *pos;
  {
    size_type j = table_entry.other() - heap_;
    while (j) {
      heap_[j] = move(heap_[parent(j)]);
      j = parent(j);
    }
    while (lchild(j) < size_) {
      if (rchild(j) < size_ && comp_(lchild(j), rchild(j))) {
        heap_[j] = move(heap_[rchild(j)]);
        j = rchild(j);
      } else {
        heap_[j] = move(heap_[lchild(j)]);
        j = lchild(j);
      }
    }
    heap_[j].~heap_type();
  }
  {
    size_type i = &table_entry - table_;
    for (size_type j = i; !free(j); j = next(j)) {
      if (((hash_at(j) - i) & bucket_mask_) <
          ((hash_at(i) - i) & bucket_mask_)) {
        table_[i] = move(table_[j]);
        set_hash_at(i, hash_at(j));
        i = j;
      }
    }
    table_[i].~table_type();
  }
}
template <typename K, typename V, typename H, typename EQ, typename C>
auto kvpq<K, V, H, EQ, C>::erase(const K& k) -> size_type {
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
  swap(bucket_mask_, o.bucket_mask_);
  swap(hash_, o.hash_);
  swap(key_equal_, o.key_equal_);
  swap(comp_, o.comp_);
  swap(size_, o.size_);
  swap(offset_, o.offset_);
  swap(table_, o.table_);
  swap(heap_, o.heap_);
}

// merge(2)
template <typename K, typename V, typename H, typename EQ, typename C>
template <typename H2, typename P2, typename C2>
void kvpq<K, V, H, EQ, C>::merge(kvpq<K, V, H2, P2, C2>&& o) {
  reserve(size_ + o.size());
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
kvpq_const_iterator<kvpq<K, V, H, EQ, C>>
kvpq<K, V, H, EQ, C>::find(const K& k) const {
  size_type h = hash_(k);
  for (size_type i = h; !free(i); i = next(i)) {
    if (hash_at(i) == h && key_eq_(table_[i].first, k)) {
      return table_[i].other();
    }
  }
  return end();
}

// Hash policy
template <typename K, typename V, typename H, typename EQ, typename C>
void kvpq<K, V, H, EQ, C>::resize(Mask bucket_mask) {
  if (~bucket_mask_ & bucket_mask) { return; }
  // new offset, table arrays

  // TODO: check that operator new[] allocates bytes, not sizeof(T) natively
  // TODO: heap_ should not be the same size as table_, but should instead grow
  // like a vector
  // TODO: don't move in copy construction / don't use table_type::make (use
  // constructor instead)?

  // iterate through offset and arrays, starting at some element after a free
  // element, moving the table value to its new position and setting the new
  // hash at and table value

  // new substruct to store bucket_mask, capacity_, offset pointer, and table
  // pointer and give it the next, etc... methods; get table_type* through
  // private table(): allows not having to duplicate next() math

  // buckets_ = buckets; (operator= for buckets calls destructor?)
  // capacity_ = get_capacity(max_load_factor_, bucket_mask_);
}

// Non-member functions
template <typename K, typename V, typename H, typename EQ, typename C>
bool kvpq<K, V, H, EQ, C>::operator==(const kvpq& o) const {
  if (this == &o) { return true; }
  if (hash_ != o.hash_ || key_equal_ != o.key_equal_ || comp_ != o.comp_ ||
      size_ != o.size_) {
    return false;
  }
  for (auto it = o.begin(); it != o.end(); ++o) {
    if (auto item = find(it->first);
        item == end() || !key_eq_(item->second, it->second)) {
      return false;
    }
  }
  return true;
}
} // namespace ds
