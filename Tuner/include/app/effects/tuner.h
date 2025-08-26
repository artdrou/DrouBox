#pragma once
#include <cstddef>
#include "effectsBase.h"
#include "audio/dsp/fft.h"
#include "cmndf.h"

class Tuner : public EffectBase {
public:
    struct effectParams {
        float sampleRate = 48000;
        int frequency = 0;
        bool bypass = false;
    };

    Tuner(ControlMapper& mapperRef)
        : EffectBase(mapperRef) {}

    void UpdateParameters();
    void Process(const float* in, float* out, size_t size) override;
    void UpdateUI() override;
    void UpdateTuningDifference();
    void UpdateTuningLeds();
    void DetectPitch(const float* input, size_t size, float sampleRate);

private:
    effectParams params_;

    int frequency_;
    int frequency;

     // Smoothing
    float smoothedFreq_ = 0.0f;
    float smoothingFactor_ = 0.5f; // 0 < factor < 1

    // Standard guitar tuning frequencies (E A D G B e)
    const std::vector<float> stringFreqs_ = {
        82.41f, 110.0f, 146.83f, 196.0f, 246.94f, 329.63
    };
    int closestString_ = -1;
    float toleranceHz_ = 3.0f; // acceptable tuning error
    int lastPeakIndex_;
    float diff_;
    size_t fftSize_;
    int maxDeviationHz_ = 50;
};