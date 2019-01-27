#pragma once

#include <tuple> // forward_as_tuple, make_from_tuple, tuple
#include <utility> // forward, move, pair, piecewise_construct, piecewise_construct_t, swap

#include "pair_fwd.hpp"

namespace intrusive {

template <typename A, typename B> class pair {
  struct private_construct_t {};
  friend class pair<B, A>;

public:
  // (47) TODO: is_constructible_v
  constexpr pair(const pair &p) = delete;
  // (58) TODO: is_constructible_v
  constexpr pair(pair &&p);

  ~pair();

  // =(12) TODO: is_assignable_v
  constexpr pair &operator=(const pair &) = delete;
  // =(34) TODO: is_assignable_v
  constexpr pair &operator=(pair &&);

  constexpr void swap(pair &);

  // constexpr void link(pair<B, A> &);

  template <typename AA, typename BB>
  static constexpr std::pair<pair, pair<B, A>> make(AA &&, BB &&);
  template <typename... AARGS, typename... BARGS>
  static constexpr std::pair<pair, pair<B, A>>
  make(std::piecewise_construct_t, std::tuple<AARGS...>, std::tuple<BARGS...>);

  constexpr bool operator==(const pair &) const;
  constexpr bool operator!=(const pair &o) const { return !(*this == o); }
  constexpr bool operator<(const pair &) const;
  constexpr bool operator<=(const pair &o) const { return !(o < *this); }
  constexpr bool operator>(const pair &o) const { return o < *this; }
  constexpr bool operator>=(const pair &o) const { return !(*this < o); }

  // std::swap()
  constexpr friend void swap(pair &lhs, pair &rhs) { lhs.swap(rhs); }

  constexpr A &get() { return _a; }
  constexpr const A &get() const { return _a; }
  constexpr operator A &() { return _a; }
  constexpr operator const A &() const { return _a; }
  constexpr A &operator*() { return _a; }
  constexpr const A &operator*() const { return _a; }
  constexpr A *operator->() { return &_a; }
  constexpr const A *operator->() const { return &_a; }

  constexpr pair<B, A> *other() { return _other; }
  constexpr const pair<B, A> *other() const { return _other; }

private:
  // (1)
  template <typename... ARGS>
  explicit constexpr pair(ARGS... args) : _a(std::forward<ARGS>(args)...) {}

  [[no_unique_address]] A _a;
  pair<B, A> *_other;
};

// (58)
template <typename A, typename B>
constexpr pair<A, B>::pair(pair &&p) : _a(std::move(p._a)), _other(p._other) {
  if (_other) {
    p._other = nullptr;
    _other->_other = this;
  }
}

template <typename A, typename B> pair<A, B>::~pair() {
  if (_other) {
    _other->_other = nullptr;
  }
}

// =(34)
template <typename A, typename B>
constexpr pair<A, B> &pair<A, B>::operator=(pair &&p) {
  if (&p == this) {
    return *this;
  }
  if (&p == _other) {
    _other = p._other = nullptr;
  }
  if (_other) {
    _other->_other = nullptr;
  }
  _other = p._other;
  _a = std::move(p._a);
  if (_other) {
    p._other = nullptr;
    _other->_other = this;
  }
  return *this;
}

template <typename A, typename B> constexpr void pair<A, B>::swap(pair &p) {
  if (&p == this) {
    return;
  }
  using std::swap;
  swap(_a, p._a);
  if (&p == _other) {
    return;
  }
  swap(_other, p._other);
  if (_other) {
    _other->_other = this;
  }
  if (p._other) {
    p._other->_other = &p;
  }
}

template <typename A, typename B>
template <typename AA, typename BB>
constexpr std::pair<pair<A, B>, pair<B, A>> pair<A, B>::make(AA &&a, BB &&b) {
  auto lhs = pair<A, B>(std::forward<AA>(a));
  auto rhs = pair<B, A>(std::forward<BB>(b));
  lhs._other = &rhs;
  rhs._other = &lhs;
  return std::make_pair(std::move(lhs), std::move(rhs));
}

template <typename A, typename B>
template <typename... AARGS, typename... BARGS>
constexpr std::pair<pair<A, B>, pair<B, A>>
pair<A, B>::make(std::piecewise_construct_t, std::tuple<AARGS...> a,
                 std::tuple<BARGS...> b) {
  auto lhs = pair<A, B>(std::make_from_tuple<A>(a));
  auto rhs = pair<B, A>(std::make_from_tuple<B>(b));
  lhs._other = &rhs;
  rhs._other = &lhs;
  return std::make_pair(std::move(lhs), std::move(rhs));
}

template <typename A, typename B>
constexpr bool pair<A, B>::operator==(const pair<A, B> &o) const {
  if (this == &o)
    return true;
  if (!(_a == o._a))
    return false;
  if (_other == o._other)
    return true;
  return _other && o._other && _other->_a == o._other->_a;
}

template <typename A, typename B>
constexpr bool pair<A, B>::operator<(const pair<A, B> &o) const {
  if (_a < o._a)
    return true;
  if (o._a < _a)
    return false;
  if (!_other && !o._other)
    return false;
  return !_other || (o._other && _other->b < o._other->b);
}
} // namespace intrusive
