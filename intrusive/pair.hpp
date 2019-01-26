#pragma once

#include <tuple> // forward_as_tuple, make_from_tuple, tuple
#include <utility> // forward, move, pair, piecewise_construct, piecewise_construct_t, swap

#include "pair_fwd.hpp"

namespace intrusive {

template <typename A, typename B> class pair {
  struct private_construct_t {};

public:
  // (1)
  constexpr pair() = default;
  // (2)
  constexpr pair(const A &a, const B &b)
      : _a(a), _other(new pair<B, A>(private_construct_t{}, *this,
                                     std::forward_as_tuple(b))) {}
  // (3) TODO: is_constructible_v
  constexpr pair(A &&a, B &&b)
      : _a(std::move(a)),
        _other(new pair<B, A>(private_construct_t{}, *this,
                              std::forward_as_tuple(std::move(b)))) {}
  // (47) TODO: is_constructible_v
  constexpr pair(const pair &p) = delete;
  // (58) TODO: is_constructible_v
  constexpr pair(pair &&p);
  // (6)
  // make_from_tuple is OK due to guaranteed copy elision
  template <typename... LHS, typename... RHS>
  constexpr pair(std::piecewise_construct_t, std::tuple<LHS...> lhs,
                 std::tuple<RHS...> rhs)
      : _a(std::make_from_tuple(std::forward(lhs))),
        _other(
            new pair<B, A>(private_construct_t{}, *this, std::forward(rhs))) {}

  ~pair();

  // =(12) TODO: is_assignable_v
  constexpr pair &operator=(const pair &) = delete;
  // =(34) TODO: is_assignable_v
  constexpr pair &operator=(pair &&);

  constexpr void swap(pair &);

  template <typename AA, typename BB>
  friend constexpr std::pair<pair<AA, BB>, pair<BB, AA>> make_pair(const AA &,
                                                                   const BB &);

  constexpr bool operator==(const pair &);
  constexpr bool operator!=(const pair &o) { return !(*this == o); }
  constexpr bool operator<(const pair &);
  constexpr bool operator<=(const pair &o) { return !(o < *this); }
  constexpr bool operator>(const pair &o) { return o < *this; }
  constexpr bool operator>=(const pair &o) { return !(*this < o); }

  // std::swap()
  friend void swap(pair &lhs, pair &rhs) { lhs.swap(rhs); }

  constexpr operator A() { return _a; }

  pair<B, A> &operator*() { return *_other; }
  const pair<B, A> &operator*() const { return *_other; }

private:
  // make_from_tuple is OK due to guaranteed copy elision
  template <typename... ARGS>
  constexpr pair(private_construct_t, pair<B, A> &other,
                 std::tuple<ARGS...> args)
      : _a(std::make_from_tuple<A, std::tuple<ARGS...>>(
            std::forward<std::tuple<ARGS...>>(args))),
        _other(&other) {}

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
  if (_other) {
    _other->_other = nullptr;
  }
  _other = p._other;
  _a = std::move(p._a);
  if (_other) {
    p._other = nullptr;
    _other->_other = this;
  }
}

template <typename A, typename B> constexpr void pair<A, B>::swap(pair &p) {
  using std::swap;
  swap(_other, p->_other);
  swap(_a, p._a);
}

template <typename A, typename B>
constexpr std::pair<pair<A, B>, pair<B, A>> make_pair(const A &a, const B &b) {
  auto p = pair<A, B>(a, b);
  return {p, *p};
}

template <typename A, typename B>
constexpr bool pair<A, B>::operator==(const pair<A, B> &o) {
  if (this == &o)
    return true;
  if (!(_a == o._a))
    return false;
  if (_other == o._other)
    return true;
  return _other && o._other && _other->_a == o->_a;
}

template <typename A, typename B>
constexpr bool pair<A, B>::operator<(const pair<A, B> &o) {
  if (_a < o._a)
    return true;
  if (o._a < _a)
    return false;
  if (!_other && !o._other)
    return false;
  return !_other || (o._other && _other->b < o->b);
}
} // namespace intrusive
