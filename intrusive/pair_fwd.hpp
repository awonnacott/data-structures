#pragma once

#include <utility> // pair

namespace intrusive {

template <typename A, typename B> class pair;

template <typename A, typename B>
constexpr std::pair<pair<A, B>, pair<B, A>> make_pair(const A &, const B &);

} // namespace intrusive
