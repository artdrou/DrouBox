#pragma once
#include <cstddef>
#include "effectsBase.h"
#include "audio/dsp/fft.h"
#include "yin.h"

class Tuner : public EffectBase {
public:
    struct effectParams {
        bool bypass = false;
    };

    Tuner(Controls& controlsRef, size_t windowSize = 4096)
        : EffectBase(controlsRef), buffer_(windowSize, 0.0f), writeIndex_(0) {}

    void UpdateParameters();
    void Process(const float* in, float* out, size_t size) override;
    void UpdateUI() override;

    void PushBlock(const float* input, size_t size);
    std::vector<float> GetBufferOrdered() const;

    void YinPitchDetection();
    void UpdateTuningLeds();

private:
    effectParams params_;
    
    Yin yin_;
    std::vector<float> buffer_;
    size_t writeIndex_;

    float frequency_;
    int sampleRate_;
    int count = 0;

     // Smoothing
    float smoothedFreq_ = 0.0f;
    float smoothingFactor_ = 0.5f; // 0 < factor < 1

    // Standard guitar tuning frequencies (E A D G B e)
    const std::vector<float> stringFreqs_ = {
        82.41f, 110.0f, 146.83f, 196.0f, 246.94f, 329.63f
    };
    int closestString_ = -1;
    float toleranceHz_ = .5f; // acceptable tuning error
    float diff_;
    int maxDeviationHz_ = 50;
};