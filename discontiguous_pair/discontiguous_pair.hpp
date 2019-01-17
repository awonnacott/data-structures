#include <tuple>   // make_from_tuple, tuple
#include <utility> // forward, move, piecewise_construct, piecewise_construct_t

template <typename A, typename B> class discontiguous_pair {
public:
  discontiguous_pair() = default;

  discontiguous_pair(const discontiguous_pair &p) = delete;
  discontiguous_pair& operator=(const discontiguous_pair &p) = delete;

  discontiguous_pair(discontiguous_pair &&p);
  discontiguous_pair& operator=(discontiguous_pair &&p);

  ~discontiguous_pair();

  discontiguous_pair<B, A> &operator*() { return *_other; }
  const discontiguous_pair<B, A> &operator*() const { return *_other; }

  operator A() const { return _a; }

  friend std::pair<discontiguous_pair, discontiguous_pair<B, A>> make_pair_discontiguous(const A&, const B&);

  discontiguous_pair(const A &a, const B &b)
      : _other(new discontiguous_pair<B, A>(*this, std::piecewise_construct,
                                            std::forward_as_tuple(b))),
        _a(a) {}

  discontiguous_pair(A &&a, B &&b)
      : _other(new discontiguous_pair<B, A>(*this, std::piecewise_construct,
                                            std::forward_as_tuple(std::move(b)))),
                                            _a(std::move(a)) {}

  // make_from_tuple is OK due to guaranteed copy elision
  template <typename... LHS, typename... RHS>
  discontiguous_pair(std::piecewise_construct_t, std::tuple<LHS...> lhs,
                     std::tuple<RHS...> rhs)
      : _other(new discontiguous_pair<B, A>(*this, std::piecewise_construct,
                                            std::forward(rhs))),
        _a(std::make_from_tuple(std::forward(lhs))) {}

private:
  /*
  discontiguous_pair& operator=(const discontiguous_pair &p) = delete;

  template <typename... ARGS>
  discontiguous_pair(discontiguous_pair<B, A> &other,
                     std::piecewise_construct_t, std::tuple<ARGS...> args)
                     : _other(&other), _a(std::make_from_tuple(std::forward(args))) {} 
  */
  discontiguous_pair<B, A> *_other{nullptr};
  A _a{};
};

template <typename A, typename B>
inline discontiguous_pair<A,B>::discontiguous_pair(discontiguous_pair &&p) : _other(p._other), _a(std::move(p._a)) {
  if (_other) {
    p._other = nullptr;
    _other->_other = this;
  }
}

template <typename A, typename B>
inline discontiguous_pair<A,B>& discontiguous_pair<A,B>::operator=(discontiguous_pair &&p) {
  if (_other) {
    _other->_other = nullptr;
  }
  _a = std::move(p._a);
  _other = p._other;
  if (_other) {
    p._other = nullptr;
    _other->_other = this;
  }
}

template <typename A, typename B>
inline discontiguous_pair<A,B>::~discontiguous_pair() {
  if (_other) {
    _other->_other = nullptr;
  }
}
