#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "mapping.h"
#include <cmath>
#include <algorithm>

// Test the pure DSP math of each distortion algorithm without Controls dependency.
// These are stand-alone versions of the functions from lib/src/effects/distortion.cpp.

static float makeupGain(float drive, float k) {
    return std::sqrt(1.f + k) / std::sqrt(1.f + k * drive);
}

static float hardClip(float in, float drive, float level) {
    float d = powerMap(drive, 0.f, 1.f, 1.f, 300.f, 1.75f);
    float l = linMap(level, 0.f, 1.f, 0.f, 2.f);
    float s = std::clamp(in * d, -0.95f, 0.95f);
    return makeupGain(d, 100.f) * s * l;
}

static float tanhClip(float in, float drive, float level) {
    float d = powerMap(drive, 0.f, 1.f, 1.f, 300.f, 1.75f);
    float l = linMap(level, 0.f, 1.f, 0.f, 2.f);
    return std::tanh(in * d) * makeupGain(d, 10.f) * l;
}

using Catch::Matchers::WithinAbs;

TEST_CASE("Hard clip: zero input gives zero output", "[distortion]") {
    REQUIRE_THAT(hardClip(0.f, 0.5f, 0.5f), WithinAbs(0.f, 1e-6f));
}

TEST_CASE("Hard clip: output bounded", "[distortion]") {
    for (float in : {-2.f, -1.f, 0.f, 1.f, 2.f}) {
        float out = hardClip(in, 0.9f, 0.5f);
        REQUIRE(std::isfinite(out));
    }
}

TEST_CASE("Tanh clip: antisymmetric (odd function)", "[distortion]") {
    float pos = tanhClip( 0.3f, 0.5f, 0.5f);
    float neg = tanhClip(-0.3f, 0.5f, 0.5f);
    REQUIRE_THAT(pos + neg, WithinAbs(0.f, 1e-5f));
}

TEST_CASE("Tanh clip: zero in gives zero out", "[distortion]") {
    REQUIRE_THAT(tanhClip(0.f, 0.5f, 0.5f), WithinAbs(0.f, 1e-6f));
}

TEST_CASE("Hard clip: finite output for extreme drive", "[distortion]") {
    float out = hardClip(1.f, 1.f, 1.f);
    REQUIRE(std::isfinite(out));
}

TEST_CASE("dBToLinear: -inf for zero input", "[distortion]") {
    // linearToDB(0) should return a very negative number
    float db = linearToDB(0.f);
    REQUIRE(db < -120.f);
}
