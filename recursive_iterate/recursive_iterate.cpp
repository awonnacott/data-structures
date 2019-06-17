#include "recursive_iterate.hpp"

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
