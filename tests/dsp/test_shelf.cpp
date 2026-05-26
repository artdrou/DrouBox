#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "shelf.h"
#include <cmath>
#include <vector>

using namespace drouAudioLib;
using Catch::Matchers::WithinAbs;

static std::vector<float> makeSine(float freq, float sr, int samples) {
    std::vector<float> out(samples);
    for (int i = 0; i < samples; ++i)
        out[i] = std::sin(2.f * 3.14159265f * freq / sr * i);
    return out;
}

static float rms(const std::vector<float>& v, int start, int end) {
    float sum = 0.f;
    for (int i = start; i < end; ++i) sum += v[i] * v[i];
    return std::sqrt(sum / (end - start));
}

TEST_CASE("Low shelf: unity gain at 0 dB boost", "[shelf]") {
    const float sr = 48000.f;
    ShelfFilter f;
    f.init(sr, 200.f, ShelfType::LOW_SHELF, 0.f, 1);

    auto sig = makeSine(1000.f, sr, 4800);
    std::vector<float> out(sig.size());
    for (size_t i = 0; i < sig.size(); ++i) out[i] = f.processFilter(sig[i]);

    float inRms  = rms(sig, 1000, 4800);
    float outRms = rms(out, 1000, 4800);
    REQUIRE_THAT(outRms / inRms, WithinAbs(1.f, 0.05f));
}

TEST_CASE("Low shelf: boosts low frequencies", "[shelf]") {
    const float sr = 48000.f;
    ShelfFilter f;
    f.init(sr, 500.f, ShelfType::LOW_SHELF, 12.f, 1);

    // Low freq (50 Hz) should be boosted
    auto lo = makeSine(50.f, sr, 4800);
    // High freq (8000 Hz) should be unaffected
    auto hi = makeSine(8000.f, sr, 4800);

    float loGain = 0.f, hiGain = 0.f;
    for (size_t i = 0; i < lo.size(); ++i) {
        float o = f.processFilter(lo[i]);
        if (i > 1000) loGain += o * o;
    }
    ShelfFilter f2;
    f2.init(sr, 500.f, ShelfType::LOW_SHELF, 12.f, 1);
    for (size_t i = 0; i < hi.size(); ++i) {
        float o = f2.processFilter(hi[i]);
        if (i > 1000) hiGain += o * o;
    }
    // Low freq power should be higher than high freq power by factor > 1
    REQUIRE(loGain > hiGain);
}

TEST_CASE("Shelf filter: no NaN or Inf output", "[shelf]") {
    const float sr = 48000.f;
    ShelfFilter f;
    f.init(sr, 1000.f, ShelfType::HIGH_SHELF, 6.f, 2);

    auto sig = makeSine(440.f, sr, 4800);
    for (float s : sig) {
        float out = f.processFilter(s);
        REQUIRE(std::isfinite(out));
    }
}

TEST_CASE("Shelf filter: stable with silent input", "[shelf]") {
    ShelfFilter f;
    f.init(48000.f, 200.f, ShelfType::LOW_SHELF, 6.f, 1);
    for (int i = 0; i < 48000; ++i) {
        float out = f.processFilter(0.f);
        REQUIRE(std::isfinite(out));
    }
}
