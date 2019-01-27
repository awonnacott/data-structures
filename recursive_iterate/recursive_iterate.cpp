// Compile with g++ a.cpp -fconcepts -std=c++2a -Wall -Wextra -Werror -lstdc++

#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>
#include <type_traits>
#include <utility>
#include <vector>

using namespace std;

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

// Demo recursive flatten
int main() {
  // The trivial flatten (int list -> int list)
  vector<int> flat{1, 2, 3, 4, 5, 6, 7, 8, 9};
  cout << equal(begin(flat), end(flat), begin(recursive_iterate<int>(flat)))
       << endl;

  // Traditional flatten (int list list -> int list)
  vector<vector<int>> two_d{vector<int>{1, 2, 3}, vector<int>{4, 5, 6},
                            vector<int>{7, 8, 9}};
  cout << equal(begin(flat), end(flat), begin(recursive_iterate<int>(two_d)))
       << endl;

  // Double flatten (int list list list -> int list
  vector<vector<vector<int>>> three_d{
      vector<vector<int>>{vector<int>{1, 2, 3}},
      vector<vector<int>>{vector<int>{4, 5, 6}},
      vector<vector<int>>{vector<int>{7, 8, 9}}};
  cout << equal(begin(flat), end(flat), begin(recursive_iterate<int>(three_d)))
       << endl;

  // Perform a traditional (single) flatten on a structure that could be double
  // flattened (int list list list -> int list list)
  vector<vector<int>> two_d_same = recursive_iterate<vector<int>>(three_d);
  if (size(two_d_same) == size(two_d)) {
    for (size_t i = 0; i < size(two_d_same); ++i) {
      if (!equal(begin(two_d[i]), end(two_d[i]), begin(two_d_same[i]))) {
        cout << false << endl;
        return 0;
      }
    }
    cout << true << endl;
  } else {
    cout << false << endl;
  }
}
