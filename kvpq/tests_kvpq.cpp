#include "kvpq.hpp"
#include <catch2/catch.hpp>
#include <iostream>
#include <variant>

using a = std::monostate;
using std::cout;
using std::endl;
using namespace ds;
TEST_CASE("", "[adding]") {
  auto p = kvpq<int, std::string>();
  REQUIRE(1 + 1 == 2);
  cout << sizeof(int) << endl;
  cout << sizeof(kvpq<int, int> *) << endl;
  cout << sizeof(kvpq<a, int>) << endl;
  cout << sizeof(a) << endl;
  cout << sizeof(kvpq<int, a>) << endl;
}
