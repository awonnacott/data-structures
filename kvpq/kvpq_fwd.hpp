// A combination of an unordered map and a priority queue
#pragma once

#include <functional>

namespace ds {
template <typename K, typename V, typename HASH = std::hash<K>,
          typename KEY_EQUAL = std::equal_to<K>,
          typename COMPARE = std::less<K>>
class kvpq;
}
