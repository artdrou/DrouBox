#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include "compressor.h"
#include <cmath>
#include <vector>

using namespace droubox;
using Catch::Matchers::WithinAbs;

TEST_CASE("Compressor: unity pass-through below threshold", "[compressor]") {
    Compressor c;
    c.Init(48000.f);
    c.SetThreshold(-6.f);  // -6 dBFS threshold
    c.SetRatio(4.f);
    c.SetBlend(1.f);

    // Input well below threshold — gain should be ~1
    float sumIn = 0.f, sumOut = 0.f;
    for (int i = 0; i < 48000; ++i) {
        float in  = 0.01f * std::sin(2.f * 3.14159f * 440.f / 48000.f * i);
        float out = c.Process(in);
        sumIn  += in * in;
        sumOut += out * out;
    }
    // Output RMS should be close to input RMS (no significant gain reduction)
    float ratio = std::sqrt(sumOut / sumIn);
    REQUIRE_THAT(ratio, WithinAbs(1.f, 0.05f));
}

TEST_CASE("Compressor: reduces gain above threshold", "[compressor]") {
    Compressor c;
    c.Init(48000.f);
    c.SetThreshold(-20.f);
    c.SetRatio(8.f);
    c.SetBlend(1.f);

    // Hot input (0 dBFS sine)
    float sumIn = 0.f, sumOut = 0.f;
    for (int i = 0; i < 48000; ++i) {
        float in  = 0.9f * std::sin(2.f * 3.14159f * 440.f / 48000.f * i);
        float out = c.Process(in);
        if (i > 4800) { // skip attack transient
            sumIn  += in * in;
            sumOut += out * out;
        }
    }
    float ratio = std::sqrt(sumOut / sumIn);
    // Output RMS should be significantly lower than input
    REQUIRE(ratio < 0.5f);
}

TEST_CASE("Compressor: output is finite for any input", "[compressor]") {
    Compressor c;
    c.Init(48000.f);
    c.SetThreshold(-24.f);
    c.SetRatio(4.f);
    c.SetBlend(1.f);

    float extremes[] = {0.f, 1.f, -1.f, 0.0001f, -0.0001f};
    for (float e : extremes) {
        float out = c.Process(e);
        REQUIRE(std::isfinite(out));
    }
}

TEST_CASE("Compressor: blend 0 passes dry signal", "[compressor]") {
    Compressor c;
    c.Init(48000.f);
    c.SetThreshold(-6.f);
    c.SetRatio(100.f); // heavy compression
    c.SetBlend(0.f);   // 100% dry

    // With blend=0, output should equal input exactly
    for (int i = 0; i < 1000; ++i) {
        float in  = 0.9f * std::sin(0.01f * i);
        float out = c.Process(in);
        REQUIRE_THAT(out, WithinAbs(in, 1e-5f));
    }
}

TEST_CASE("Compressor: no output with silent input", "[compressor]") {
    Compressor c;
    c.Init(48000.f);
    c.SetThreshold(-24.f);
    c.SetBlend(1.f);

    for (int i = 0; i < 1000; ++i) {
        float out = c.Process(0.f);
        REQUIRE_THAT(out, WithinAbs(0.f, 1e-4f));
    }
}
