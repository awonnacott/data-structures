// A combination of an unordered map and a priority queue
#pragma once

#include <functional>

namespace ds::impl {
template <typename K, typename V, typename HASH = std::hash<K>,
          typename KEY_EQUAL = std::equal_to<K>,
          typename COMPARE = std::less<K>>
class kvpq_traits;
template <typename TRAITS> class kvpq;
} // namespace ds::impl

namespace ds {
template <typename... ARGS>
using kvpq = ds::impl::kvpq<ds::impl::kvpq_traits<ARGS...>>;
}
