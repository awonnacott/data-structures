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
  constexpr pair(const pair& p) = delete;
  // (58) TODO: is_constructible_v
  constexpr pair(pair&& p);

  ~pair();

  // =(12) TODO: is_assignable_v
  constexpr pair& operator=(const pair&) = delete;
  // =(34) TODO: is_assignable_v
  constexpr pair& operator=(pair&&);

  constexpr void swap(pair&);

  template <typename AA, typename BB>
  static constexpr std::pair<pair, pair<B, A>> make(AA&&, BB&& = BB());
  template <typename... AARGS, typename... BARGS>
  static constexpr std::pair<pair, pair<B, A>> make(std::piecewise_construct_t,
                                                    std::tuple<AARGS...>,
                                                    std::tuple<BARGS...>);

  constexpr bool operator==(const pair&) const;
  constexpr bool operator!=(const pair& o) const { return !(*this == o); }
  constexpr bool operator<(const pair&) const;
  constexpr bool operator<=(const pair& o) const { return !(o < *this); }
  constexpr bool operator>(const pair& o) const { return o < *this; }
  constexpr bool operator>=(const pair& o) const { return !(*this < o); }

  // std::swap()
  constexpr friend void swap(pair& lhs, pair& rhs) { lhs.swap(rhs); }

  constexpr A& get() { return a_; }
  constexpr const A& get() const { return a_; }
  constexpr operator A&() { return a_; }
  constexpr operator const A&() const { return a_; }
  constexpr A& operator*() { return a_; }
  constexpr const A& operator*() const { return a_; }
  constexpr A* operator->() { return &a_; }
  constexpr const A* operator->() const { return &a_; }

  constexpr pair<B, A>* other() { return other_; }
  constexpr const pair<B, A>* other() const { return other_; }

 private:
  // (1)
  template <typename... ARGS>
  constexpr explicit pair(ARGS... args)
      : a_(std::forward<ARGS>(args)...), other_(nullptr) {}

  [[no_unique_address]] A a_;
  pair<B, A>* other_;
};

// (58)
template <typename A, typename B>
constexpr pair<A, B>::pair(pair&& p) : a_(std::move(p.a_)), other_(p.other_) {
  if (other_) {
    p.other_ = nullptr;
    other_->other_ = this;
  }
}

template <typename A, typename B> pair<A, B>::~pair() {
  if (other_) { other_->other_ = nullptr; }
}

// =(34)
template <typename A, typename B>
constexpr pair<A, B>& pair<A, B>::operator=(pair&& p) {
  if (&p == this) { return *this; }
  if (&p == other_) { other_ = p.other_ = nullptr; }
  if (other_) { other_->other_ = nullptr; }
  other_ = p.other_;
  a_ = std::move(p.a_);
  if (other_) {
    p.other_ = nullptr;
    other_->other_ = this;
  }
  return *this;
}

template <typename A, typename B> constexpr void pair<A, B>::swap(pair& p) {
  if (&p == this) { return; }
  using std::swap;
  swap(a_, p.a_);
  if (&p == other_) { return; }
  swap(other_, p.other_);
  if (other_) { other_->other_ = this; }
  if (p.other_) { p.other_->other_ = &p; }
}

template <typename A, typename B>
template <typename AA, typename BB>
constexpr std::pair<pair<A, B>, pair<B, A>> pair<A, B>::make(AA&& a, BB&& b) {
  auto lhs = pair<A, B>(std::forward<AA>(a));
  auto rhs = pair<B, A>(std::forward<BB>(b));
  lhs.other_ = &rhs;
  rhs.other_ = &lhs;
  return std::make_pair(std::move(lhs), std::move(rhs));
}

template <typename A, typename B>
template <typename... AARGS, typename... BARGS>
constexpr std::pair<pair<A, B>, pair<B, A>>
pair<A, B>::make(std::piecewise_construct_t, std::tuple<AARGS...> a,
                 std::tuple<BARGS...> b) {
  auto lhs = pair<A, B>(std::make_from_tuple<A>(a));
  auto rhs = pair<B, A>(std::make_from_tuple<B>(b));
  lhs.other_ = &rhs;
  rhs.other_ = &lhs;
  return std::make_pair(std::move(lhs), std::move(rhs));
}

template <typename A, typename B>
constexpr bool pair<A, B>::operator==(const pair<A, B>& o) const {
  if (this == &o) { return true; }
  if (!(a_ == o.a_)) { return false; }
  if (other_ == o.other_) { return true; }
  return other_ && o.other_ && other_->a_ == o.other_->a_;
}

template <typename A, typename B>
constexpr bool pair<A, B>::operator<(const pair<A, B>& o) const {
  if (a_ < o.a_) { return true; }
  if (o.a_ < a_) { return false; }
  if (!other_ && !o.other_) { return false; }
  return !other_ || (o.other_ && other_->b < o.other_->b);
}
} // namespace intrusive
