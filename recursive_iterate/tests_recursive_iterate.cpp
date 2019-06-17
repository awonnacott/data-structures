#include "recursive_iterate.hpp"
#include <catch2/catch.hpp>

using namespace std;

const vector<int> flat{1, 2, 3, 4, 5, 6, 7, 8, 9};

TEST_CASE("The trivial flatten (int list -> int list)", "[recursive_iterate]") {
  REQUIRE(equal(begin(flat), end(flat), begin(recursive_iterate<int>(flat))));
}

const vector<vector<int>> two_d{vector<int>{1, 2, 3}, vector<int>{4, 5, 6},
                                vector<int>{7, 8, 9}};

TEST_CASE("Traditional flatten (int list list -> int list)",
          "[recursive_iterate]") {
  REQUIRE(equal(begin(flat), end(flat), begin(recursive_iterate<int>(two_d))));
}

const vector<vector<vector<int>>> three_d{
    vector<vector<int>>{vector<int>{1, 2, 3}},
    vector<vector<int>>{vector<int>{4, 5, 6}},
    vector<vector<int>>{vector<int>{7, 8, 9}}};

TEST_CASE("Double flatten (int list list list -> int list)",
          "[recursive_iterate]") {
  REQUIRE(
      equal(begin(flat), end(flat), begin(recursive_iterate<int>(three_d))));
}
TEST_CASE("Perform a traditional (single) flatten on a structure that could be "
          "double flattened (int list list list -> int list list)",
          "[recursive_iterate]") {
  const vector<vector<int>> two_d_same =
      recursive_iterate<vector<int>>(three_d);
  REQUIRE(size(two_d_same) == size(two_d));
  for (size_t i = 0; i < size(two_d_same); ++i) {
    REQUIRE(equal(begin(two_d[i]), end(two_d[i]), begin(two_d_same[i])));
  }
}
