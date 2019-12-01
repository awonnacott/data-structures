#include <catch2/catch.hpp>
#include <iostream>
#include <limits>
#include <variant>

#include "kvpq.hpp"

using ds::kvpq;
using IntStringKvpq = kvpq<int, std::string>;

TEST_CASE("constructor kvpq<int,string>", "[kvpq]") {
  IntStringKvpq p;
  REQUIRE(p.begin() == p.begin());
  REQUIRE(p.begin() == p.end());
  REQUIRE(++p.begin() - 1 == p.begin());
}

TEST_CASE("find", "[kvpq]") {
  IntStringKvpq p(35);
  REQUIRE(p.capacity() == 64);
}
