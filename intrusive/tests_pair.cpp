#include "pair.hpp"
#include <catch2/catch.hpp>
#include <iostream>
#include <variant>

using a = std::monostate;
using std::cout;
using std::endl;
using namespace intrusive;
TEST_CASE("", "[adding]") {
  auto p = pair(3, 4);
  REQUIRE(1 + 1 == 2);
  cout << sizeof(int) << endl;
  cout << sizeof(pair<int, int> *) << endl;
  cout << sizeof(pair<a, int>) << endl;
  cout << sizeof(a) << endl;
  cout << sizeof(pair<int, a>) << endl;
}
