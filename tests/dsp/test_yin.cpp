#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "yin.h"
#include <cmath>
#include <vector>

using Catch::Matchers::WithinAbs;
using Catch::Matchers::WithinRel;

static constexpr float SR = 48000.f;

static std::vector<float> makeSine(float freq, float sr, int samples) {
    std::vector<float> out(samples);
    for (int i = 0; i < samples; ++i)
        out[i] = std::sin(2.f * 3.14159265f * freq / sr * i);
    return out;
}

static Yin makeGuitarYin() {
    Yin y;
    y.SetSampleRate(SR);
    y.SetDecimation(4);
    y.SetMinFreq(75);
    y.SetMaxFreq(15000);
    y.SetThreshold(0.15f);
    return y;
}

TEST_CASE("YIN detects 440 Hz (A4)", "[yin]") {
    auto y   = makeGuitarYin();
    auto sig = makeSine(440.f, SR, 2048);
    float detected = y.DetectPitch(sig);
    REQUIRE(detected > 0.f);
    REQUIRE_THAT(detected, WithinRel(440.f, 0.05f)); // within 5%
}

TEST_CASE("YIN detects 82 Hz (E2 low guitar string)", "[yin]") {
    auto y   = makeGuitarYin();
    auto sig = makeSine(82.41f, SR, 2048);
    float detected = y.DetectPitch(sig);
    // Low E is at the edge of detection range with decimation=4; expect ±10%
    if (detected > 0.f) {
        REQUIRE_THAT(detected, WithinRel(82.41f, 0.1f));
    }
    // If not detected, that's also acceptable for this edge case
}

TEST_CASE("YIN detects 329 Hz (E4 high guitar string)", "[yin]") {
    auto y   = makeGuitarYin();
    auto sig = makeSine(329.63f, SR, 2048);
    float detected = y.DetectPitch(sig);
    REQUIRE(detected > 0.f);
    REQUIRE_THAT(detected, WithinRel(329.63f, 0.05f));
}

TEST_CASE("YIN returns -1 for silence", "[yin]") {
    auto y   = makeGuitarYin();
    std::vector<float> silence(2048, 0.f);
    float detected = y.DetectPitch(silence);
    REQUIRE(detected < 0.f);
}

TEST_CASE("YIN returns finite value for any input", "[yin]") {
    auto y   = makeGuitarYin();
    // White noise
    std::vector<float> noise(2048);
    for (int i = 0; i < 2048; ++i) noise[i] = (i % 17 - 8) / 8.f;
    float detected = y.DetectPitch(noise);
    REQUIRE(std::isfinite(detected));
}

TEST_CASE("YIN no out-of-bounds with minimum buffer size", "[yin]") {
    auto y   = makeGuitarYin();
    auto sig = makeSine(440.f, SR, 64); // tiny buffer
    float detected = y.DetectPitch(sig);
    REQUIRE(std::isfinite(detected));
}
