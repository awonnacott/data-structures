#include <tuple> // forward_as_tuple, make_from_tuple, tuple
#include <utility> // forward, move, pair, piecewise_construct, piecewise_construct_t, swap

#include "pair_fwd.hpp"

namespace intrusive {

template <typename A, typename B> class pair : public A {
  struct private_construct_t {};

public:
  // (1)
  constexpr pair() = default;
  // (2)
  constexpr pair(const A &a, const B &b)
      : A(a), _other(new pair<B, A>(private_construct_t{}, *this,
                                    std::forward_as_tuple(b))) {}
  // (3) TODO: is_constructible_v
  constexpr pair(A &&a, B &&b)
      : A(std::move(a)),
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
      : A(std::make_from_tuple(std::forward(lhs))),
        _other(
            new pair<B, A>(private_construct_t{}, *this, std::forward(rhs))) {}

  ~pair();

  // =(12) TODO: is_assignable_v
  constexpr pair &operator=(const pair &) = delete;
  // =(34) TODO: is_assignable_v
  constexpr pair &operator=(pair &&);

  constexpr void swap(pair &);

  constexpr friend std::pair<pair, pair<B, A>> make_pair(const A &, const B &);

  constexpr friend bool operator==(const pair &, const pair &);
  constexpr friend bool operator!=(const pair &lhs, const pair &rhs) {
    return !(lhs == rhs);
  }
  constexpr friend bool operator<(const pair &, const pair &);
  constexpr friend bool operator<=(const pair &lhs, const pair &rhs) {
    return !(rhs < lhs);
  }
  constexpr friend bool operator>(const pair &lhs, const pair &rhs) {
    return rhs < lhs;
  }
  constexpr friend bool operator>=(const pair &lhs, const pair &rhs) {
    return !(lhs < rhs);
  }

  // std::swap()
  friend void swap(pair &lhs, pair &rhs) { lhs.swap(rhs); }

  pair<B, A> &operator*() { return *_other; }
  const pair<B, A> &operator*() const { return *_other; }

private:
  // make_from_tuple is OK due to guaranteed copy elision
  template <typename... ARGS>
  constexpr pair(private_construct_t, pair<B, A> &other,
                 std::tuple<ARGS...> args)
      : A(std::make_from_tuple<A, std::tuple<ARGS...>>(
            std::forward<std::tuple<ARGS...>>(args))),
        _other(&other) {}

  pair<B, A> *_other;
};

// (58)
template <typename A, typename B>
constexpr pair<A, B>::pair(pair &&p) : A(std::move(p._a)), _other(p._other) {
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
  static_cast<A &>(*this) = std::move(p);
  if (_other) {
    p._other = nullptr;
    _other->_other = this;
  }
}

template <typename A, typename B> constexpr void pair<A, B>::swap(pair &p) {
  using std::swap;
  swap(_other, p->_other);
  swap(static_cast<A &>(*this), static_cast<A &>(p));
}

template <typename A, typename B>
std::pair<pair<A, B>, pair<B, A>> make_pair(const A &a, const B &b) {
  auto p = pair<A, B>(a, b);
  return {p, *p};
}

template <typename A, typename B>
constexpr bool operator==(const pair<A, B> &lhs, const pair<A, B> &rhs) {
  if (&lhs == &rhs)
    return true;
  if (!(static_cast<const A &>(lhs) == static_cast<const A &>(rhs)))
    return false;
  if (lhs._other == rhs._other)
    return true;
  return lhs._other && rhs._other &&
         static_cast<const B &>(*lhs) == static_cast<const B &>(*rhs);
}

template <typename A, typename B>
constexpr bool operator<(const pair<A, B> &lhs, const pair<A, B> &rhs) {
  if (static_cast<const A &>(lhs) < static_cast<const A &>(rhs))
    return true;
  if (static_cast<const A &>(rhs) < static_cast<const A &>(lhs))
    return false;
  if (!lhs._other && !rhs._other)
    return false;
  return !lhs._other || (rhs._other && static_cast<const B &>(*lhs) <
                                           static_cast<const B &>(*rhs));
}
} // namespace intrusive
