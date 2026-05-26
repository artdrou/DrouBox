#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "lfo.h"
#include <cmath>

using namespace drouAudioLib;
using Catch::Matchers::WithinAbs;
using Catch::Matchers::WithinRel;

static constexpr float SR = 48000.f;

TEST_CASE("LFO sine output stays in [-1, 1]", "[lfo]") {
    Lfo lfo;
    lfo.init(SR, 1.f, Lfo::Waveform::Sine);
    for (int i = 0; i < 48000; ++i) {
        float s = lfo.getNextSample();
        REQUIRE(s >= -1.001f);
        REQUIRE(s <=  1.001f);
    }
}

TEST_CASE("LFO triangle output stays in [-1, 1]", "[lfo]") {
    Lfo lfo;
    lfo.init(SR, 2.f, Lfo::Waveform::Triangle);
    for (int i = 0; i < 48000; ++i) {
        float s = lfo.getNextSample();
        REQUIRE(s >= -1.001f);
        REQUIRE(s <=  1.001f);
    }
}

TEST_CASE("LFO square output is +1 or -1", "[lfo]") {
    Lfo lfo;
    lfo.init(SR, 1.f, Lfo::Waveform::Square);
    for (int i = 0; i < 48000; ++i) {
        float s = lfo.getNextSample();
        REQUIRE((std::fabs(s - 1.f) < 0.001f || std::fabs(s + 1.f) < 0.001f));
    }
}

TEST_CASE("LFO sine completes correct number of cycles", "[lfo]") {
    const float freq = 10.f;
    Lfo lfo;
    lfo.init(SR, freq, Lfo::Waveform::Sine);

    int zeroCrossings = 0;
    float prev = 0.f;
    for (int i = 0; i < static_cast<int>(SR); ++i) {
        float cur = lfo.getNextSample();
        // Count positive-going zero crossings
        if (prev <= 0.f && cur > 0.f) ++zeroCrossings;
        prev = cur;
    }
    // Should have ~freq zero crossings per second
    REQUIRE(zeroCrossings >= static_cast<int>(freq) - 1);
    REQUIRE(zeroCrossings <= static_cast<int>(freq) + 1);
}

TEST_CASE("LFO phase is finite for high frequencies", "[lfo]") {
    // High frequency that would break simple `phase -= 1.0` wraparound
    Lfo lfo;
    lfo.init(SR, 24000.f, Lfo::Waveform::Sine);
    for (int i = 0; i < 100; ++i) {
        float s = lfo.getNextSample();
        REQUIRE(std::isfinite(s));
    }
}
