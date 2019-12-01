#include <catch2/catch.hpp>
#include <iostream>
#include <limits>
#include <variant>

#define private public

#include "kvpq.hpp"

using ds::kvpq;
using IntStringKvpq = kvpq<int, std::string>;
using std::size_t;

using Mask = IntStringKvpq::Mask;
float get_load_factor(size_t size, Mask bucket_mask) {
  return IntStringKvpq::get_load_factor(size, bucket_mask);
}
Mask get_bucket_mask(size_t size, float load_factor) {
  return IntStringKvpq::get_bucket_mask(size, load_factor);
}
size_t get_capacity(float load_factor, Mask bucket_mask) {
  return IntStringKvpq::get_capacity(load_factor, bucket_mask);
}

TEST_CASE("mask", "[kvpq]") {
  REQUIRE(size_t(Mask(1)) == 1);
  REQUIRE(size_t(Mask(2)) == 3);
  REQUIRE(size_t(Mask(5)) == 7);
  for (uint i = 1; i < 6; ++i) {
    for (uint j = 1 << i; j < 1 << (i + 1); ++j) {
      REQUIRE(size_t(Mask(j)) == (1 << (i + 1)) - 1);
    }
  }
}

TEST_CASE("static_load_factor", "[kvpq]") {
  REQUIRE(get_load_factor(0, Mask(1)) == 0.);
  REQUIRE(get_bucket_mask(0, 0.) == Mask(1));
  REQUIRE(get_capacity(0., Mask(1)) == 0);

  REQUIRE(get_load_factor(1, Mask(1)) == 1.5);
  REQUIRE(get_bucket_mask(1, 1.5) == Mask(1));
  REQUIRE(get_capacity(1.5, Mask(1)) == 1);

  REQUIRE(get_load_factor(2, Mask(1)) ==
          std::numeric_limits<float>::infinity());
  REQUIRE(get_capacity(std::numeric_limits<float>::infinity(), Mask(1)) == 2);

  REQUIRE(get_load_factor(256, Mask(255)) ==
          std::numeric_limits<float>::infinity());
  REQUIRE(get_capacity(std::numeric_limits<float>::infinity(), Mask(255)) ==
          256);

  REQUIRE(get_load_factor(0, Mask(255)) == 0.);
  REQUIRE(get_capacity(0., Mask(255)) == 0);

  REQUIRE(get_load_factor(7, Mask(7)) == 31.5);
  REQUIRE(get_bucket_mask(7, 31.5) == Mask(7));
  REQUIRE(get_capacity(31.5, Mask(7)) == 7);

  REQUIRE(get_bucket_mask(1, 1.) == Mask(3));
  REQUIRE(get_capacity(1., Mask(3)) == 1);

  REQUIRE(get_load_factor(5, Mask(15)) <= 1.);
  REQUIRE(get_bucket_mask(5, 1.) == Mask(15));
  REQUIRE(get_capacity(1., Mask(15)) == 6);

  REQUIRE(get_bucket_mask(1, 5.) == Mask(1));
  REQUIRE(get_bucket_mask(5, 5.) == Mask(7));
  REQUIRE(get_bucket_mask(1, 0.2) == Mask(7));
  REQUIRE(get_bucket_mask(5, 0.2) == Mask(63));

  REQUIRE(get_bucket_mask(0, 1.) == Mask(1));
  REQUIRE(get_capacity(1., Mask(1)) == 0);

  REQUIRE(get_load_factor(3, Mask(7)) <= 1.);
  REQUIRE(get_bucket_mask(3, 1.) == Mask(7));
  REQUIRE(get_capacity(1., Mask(7)) == 3);

  REQUIRE(get_capacity(5., Mask(1)) == 1);
  REQUIRE(get_capacity(5., Mask(3)) == 2);
  REQUIRE(get_capacity(5., Mask(7)) == 5);
  REQUIRE(get_capacity(5., Mask(15)) == 11);
  REQUIRE(get_capacity(0.2, Mask(1)) == 0);
  REQUIRE(get_capacity(0.2, Mask(3)) == 0);
  REQUIRE(get_capacity(0.2, Mask(7)) == 1);
  REQUIRE(get_capacity(0.2, Mask(15)) == 2);
}
