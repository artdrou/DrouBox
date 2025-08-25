#pragma once
#include <cstddef>
#include "effectsBase.h"
#include "audio/dsp/fft.h"

class Tuner : public EffectBase {
public:
    struct effectParams {
        float sampleRate = 48000;
        int smoothedFreq = 0;
        bool bypass = false;
    };

    Tuner() {}

    void UpdateParameters(const ControlMapper& mapper);
    void Process(const float* in, float* out, size_t size) override;
    void UpdateUI(Controls& controls) override;
    void UpdateTuningDifference(Controls& controls);
    void UpdateTuningLeds(Controls& controls);
    void FindFrequency();

    effectParams GetParams() const;

private:
     effectParams params_;

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
};