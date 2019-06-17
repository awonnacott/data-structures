#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>
#include <type_traits>
#include <utility>
#include <vector>

using std::declval;
using std::is_same_v;
using std::remove_cvref_t;
using std::vector;

// begin C++20 stuff not yet in GCC
template <typename T, typename U> concept bool Same = is_same_v<T, U>;
// end C++20 stuff

template <typename CONTAINER> concept bool Iterable = requires(CONTAINER t) {
  {*begin(t)};
  {*end(t)};
  requires Same<decltype(*begin(t)), decltype(*end(t))>;
};

template <typename CONTAINER>
requires Iterable<CONTAINER> using Contents =
    remove_cvref_t<decltype(*begin(declval<CONTAINER>()))>;

template <typename CONTAINER, typename CONTENTS>
requires Iterable<CONTAINER> concept bool Contains =
    Same<Contents<CONTAINER>, CONTENTS>;

template <typename CONTAINER, typename CONTENTS>
requires Iterable<CONTAINER> concept bool RecursiveContains =
    Contains<CONTAINER, CONTENTS> ||
    RecursiveContains<Contents<CONTAINER>, CONTENTS>;

template <typename CONTENTS, typename CONTAINER>
requires RecursiveContains<CONTAINER, CONTENTS> vector<CONTENTS>
recursive_iterate(CONTAINER container) {
  if constexpr (Contains<CONTAINER, CONTENTS>) {
    return vector<CONTENTS>(container.begin(), container.end());
  } else {
    vector<CONTENTS> contents;
    for (Contents<CONTAINER>& subcontainer : container) {
      vector<CONTENTS> subcontents = recursive_iterate<CONTENTS>(subcontainer);
      contents.insert(contents.end(), subcontents.begin(), subcontents.end());
    }
    return contents;
  }
}
