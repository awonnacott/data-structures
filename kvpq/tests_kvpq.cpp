#include "kvpq.hpp"
#include <catch2/catch.hpp>
#include <iostream>
#include <variant>

using std::cout;
using std::endl;
using namespace ds;

TEST_CASE("constructor kvpq<int,string", "[kvpq]") {
  auto p = kvpq<int, std::string>();
  REQUIRE(p.begin() == p.begin());
  REQUIRE(p.begin() == p.end());
  REQUIRE(++p.begin() - 1 == p.begin());
}

TEST_CASE("find", "[kvpq]") {
  auto p = kvpq<int, std::string>(35);
  REQUIRE(p.capacity() == 64);
}
