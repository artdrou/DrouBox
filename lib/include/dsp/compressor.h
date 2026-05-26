#pragma once
#include <cmath>

namespace droubox {

class Compressor {
public:
    void Init(float sampleRate);
    void SetThreshold(float thresh_dB);
    void SetRatio(float ratio);
    void SetAttack(float attack_ms);
    void SetRelease(float release_ms);
    void SetMakeupGain(float gain_dB);
    void SetBlend(float blend);
    void SetKneeWidth(float kneeWidth_dB);
    void SetUseRMS(bool enable);

    // Convenience: map three knobs to threshold/ratio/attack/blend
    void UpdateFromKnobs(float sustainKnob, float attackKnob, float blendKnob);

    float Process(float in);

private:
    float sampleRate_ = 48000.f;
    float threshold_  = 0.063f; // -24 dBFS in linear
    float ratio_      = 3.f;
    float attackCoeff_  = 0.f;
    float releaseCoeff_ = 0.f;
    float makeupGain_ = 1.f;
    float envelope_   = 0.f;
    float kneeWidth_  = 6.f;
    float blend_      = 1.f;
    bool  useRMS_     = false;
    float rmsWindow_  = 0.f;

    void  updateCoeffs();
    float calcEnvelope(float level);
    float calcGainReduction(float envelope);
};

} // namespace droubox
