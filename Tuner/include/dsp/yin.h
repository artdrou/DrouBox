#pragma once
#include <vector>
#include <cstddef>
#include "controls.h"

class Yin {
public:
    Yin() = default;

    // setters
    void SetSampleRate(float sr) {
        sampleRate_ = sr;
        SetDecimation(decimationFactor_);
    }
    void SetMinFreq(int f) {
        minFreq_ = f;
        maxTau_ = decimatedSampleRate_/minFreq_;
    }
    void SetMaxFreq(int f) {
        maxFreq_ = f;
        minTau_ = decimatedSampleRate_/maxFreq_;
    }
    void SetDecimation(size_t d) {
        decimationFactor_ = d;
        decimatedSampleRate_ = sampleRate_/(float)decimationFactor_;
        SetMinFreq(minFreq_);
        SetMaxFreq(maxFreq_);
    }
    void SetThreshold(float t) { threshold_ = t; }
    void SetCorrectionFactor(float cf) { correctionFactor_ = cf; }

    // Main pitch detection entry point
    float DetectPitch(const std::vector<float>& in);

private:
    float sampleRate_ = 48000.f;
    int minFreq_ = 1;
    int maxFreq_ = 20000;
    int maxTau_ = 4096;
    int minTau_ = 0;
    float threshold_ = 0.15f;
    size_t decimationFactor_ = 8;
    float correctionFactor_ = 1.0f;
    float decimatedSampleRate_ = 6000.f;

    // Internal steps
    std::vector<float> NormalizeBuffer(const std::vector<float>& in);
    std::vector<float> Decimate(const std::vector<float>& input);
    std::vector<float> DifferenceFunction(const std::vector<float>& normalizedBuffer);
    std::vector<float> CumulativeMeanNormalizedDifference(const std::vector<float>& diff);
    float AbsoluteThreshold(const std::vector<float>& cmndf);
    float ParabolicInterpolation(const std::vector<float>& cmndf, float tau);
    float GetPitchFromLag(int tau);
};

