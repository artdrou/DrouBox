#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "mapping.h"
#include <cmath>

using Catch::Matchers::WithinAbs;

TEST_CASE("linMap midpoint", "[mapping]") {
    float result = linMap(0.5f, 0.f, 1.f, 0.f, 100.f);
    REQUIRE_THAT(result, WithinAbs(50.f, 0.001f));
}

TEST_CASE("linMap min/max", "[mapping]") {
    REQUIRE_THAT(linMap(0.f, 0.f, 1.f, 10.f, 20.f), WithinAbs(10.f, 0.001f));
    REQUIRE_THAT(linMap(1.f, 0.f, 1.f, 10.f, 20.f), WithinAbs(20.f, 0.001f));
}

TEST_CASE("logMap returns min at input min", "[mapping]") {
    float result = logMap(0.f, 0.f, 1.f, 20.f, 20000.f);
    REQUIRE_THAT(result, WithinAbs(20.f, 0.01f));
}

TEST_CASE("logMap returns max at input max", "[mapping]") {
    float result = logMap(1.f, 0.f, 1.f, 20.f, 20000.f);
    REQUIRE_THAT(result, WithinAbs(20000.f, 1.f));
}

TEST_CASE("dBToLinear / linearToDB round-trip", "[mapping]") {
    for (float db : {-60.f, -24.f, 0.f, 6.f, 12.f}) {
        float lin    = dBToLinear(db);
        float db_rt  = linearToDB(lin);
        REQUIRE_THAT(db_rt, WithinAbs(db, 0.001f));
    }
}

TEST_CASE("dBToLinear 0dB is 1.0", "[mapping]") {
    REQUIRE_THAT(dBToLinear(0.f), WithinAbs(1.f, 1e-5f));
}

TEST_CASE("dBToLinear 20dB is 10.0", "[mapping]") {
    REQUIRE_THAT(dBToLinear(20.f), WithinAbs(10.f, 0.001f));
}

TEST_CASE("powerMap midpoint with exponent 2", "[mapping]") {
    float result = powerMap(0.5f, 0.f, 1.f, 0.f, 100.f, 2.f);
    // 0.5^2 = 0.25, so result = 0 + 0.25 * 100 = 25
    REQUIRE_THAT(result, WithinAbs(25.f, 0.001f));
}
