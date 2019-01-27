#include "pair.hpp"
#include <catch2/catch.hpp>
#include <variant>

using namespace intrusive;
TEST_CASE("pairs of ints", "[intrusive::pair]") {
  auto [p, q] = pair<int, int>::make(3, 4);
  REQUIRE(p == 3);
  REQUIRE(4 == q);
  REQUIRE(p.other()->get() == 4);
  REQUIRE(3 == q.other()->get());
  REQUIRE(sizeof(pair<int, int> *) == sizeof(size_t));
  REQUIRE(sizeof(pair<int, int>) == 2 * sizeof(size_t));
  auto [r, s] = pair<int, int>::make(5, 6);
  REQUIRE(p == 3);
  REQUIRE(s.other()->get() == 5);
  REQUIRE(p.other()->get() == 4);
  REQUIRE(r.other()->get() == 6);
  REQUIRE(q.other() == &p);
  REQUIRE(p.other() == &q);
  REQUIRE(r.other() == &s);
  REQUIRE(s.other() == &r);
}
TEST_CASE("pairs of monostates", "[intrusive::pair]") {
  REQUIRE(sizeof(pair<int, std::monostate>) == 2 * sizeof(size_t));
  REQUIRE(sizeof(pair<std::monostate, int>) == sizeof(size_t));
  REQUIRE(sizeof(pair<std::monostate, std::monostate>) == sizeof(size_t));
  auto [p, q] = pair<std::pair<int, std::string>, std::monostate>::make(
      std::make_pair<int, std::string>(3, "abcd"), std::monostate());
  auto [r, s] = pair<std::pair<int, std::string>, std::monostate>::make(
      std::piecewise_construct, std::make_tuple(3, "abcd"), std::make_tuple());
}
TEST_CASE("pairs of strings and containers", "[intrusive::pair]") {
  auto [p, q] = pair<std::string, std::string>::make("Left", "Right");
  REQUIRE(p.get() == "Left");
  REQUIRE("Right" == q.get());
}
TEST_CASE("swapping", "[intrusive::pair]") {
  auto [p, q] = pair<int, int>::make(3, 4);
  auto [r, s] = pair<int, int>::make(5, 6);
  using std::swap;
  swap(p, r);
  REQUIRE(q.other() == &r);
  REQUIRE(r.other() == &q);
  REQUIRE(p.other() == &s);
  REQUIRE(s.other() == &p);
  REQUIRE(p == 5);
  REQUIRE(p.other()->get() == 6);
  REQUIRE(r.other()->get() == 4);
  REQUIRE(q.other()->get() == 3);
  REQUIRE(r == 3);
  REQUIRE(s.other()->get() == 5);
}

TEST_CASE("moving", "[intrusive::pair]") {
  auto [p, q] = pair<int, int>::make(3, 4);
  auto [r, s] = pair<int, int>::make(5, 6);
  r = std::move(p);
  REQUIRE(q.other() == &r);
  REQUIRE(r.other() == &q);
  REQUIRE(p.other() == nullptr);
  REQUIRE(s.other() == nullptr);
  REQUIRE(r == 3);
  REQUIRE(q == 4);
  REQUIRE(s == 6);
}
