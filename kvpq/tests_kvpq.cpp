#include "kvpq.hpp"
#include <catch2/catch.hpp>
#include <iostream>
#include <variant>

using a = std::monostate;
using std::cout;
using std::endl;
using namespace ds;
TEST_CASE("kvpq<int>", "[kvpq]") {
  auto p = kvpq<int, std::string>();
  REQUIRE(1 + 1 == 2);
  REQUIRE(p.begin() == p.begin());
  REQUIRE(p.begin() == p.end());
  REQUIRE(++p.begin() - 1 == p.begin());
}
