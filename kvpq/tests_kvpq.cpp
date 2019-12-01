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
  // REQUIRE(++p.begin() - 1 == p.begin());
}

TEST_CASE("find", "[kvpq]") {
  IntStringKvpq p(35);
  REQUIRE(p.capacity() == 64);
  REQUIRE(p.find(4) == p.end());
}

TEST_CASE("insert", "[kvpq]") {
  IntStringKvpq p(3);
  REQUIRE(p.capacity() == 4);
  REQUIRE(p.empty());
  REQUIRE(p.size() == 0);

  p.insert({3, "abcd"});
  REQUIRE(p.find(3) != p.end());
  REQUIRE(p.find(4) == p.end());
  REQUIRE(p.size() == 1);
  REQUIRE(p.capacity() == 4);
  REQUIRE(p[3] == "abcd");
  REQUIRE(p.size() == 1);

  p.insert({2, "bcd"});
  REQUIRE(p.find(2) != p.end());
  REQUIRE(p.find(3) != p.end());
  REQUIRE(p.find(4) == p.end());
  REQUIRE(p.size() == 2);
  REQUIRE(p.capacity() == 4);
  REQUIRE(p[2] == "bcd");
  REQUIRE(p[3] == "abcd");
  REQUIRE(p.size() == 2);

  p.insert({5, "cd"});
  REQUIRE(p.find(2) != p.end());
  REQUIRE(p.find(3) != p.end());
  REQUIRE(p.find(4) == p.end());
  REQUIRE(p.find(5) != p.end());
  REQUIRE(p.size() == 3);
  REQUIRE(p.capacity() == 4);
  REQUIRE(p[2] == "bcd");
  REQUIRE(p[3] == "abcd");
  REQUIRE(p[5] == "cd");
  REQUIRE(p.size() == 3);
}
