#pragma once
#include "effectsBase.h"
#include "lfo.h"
#include "dc_block.h"
#include <vector>

namespace droubox {

struct DelayConfig {
    size_t knobTimeL   = 0;
    size_t knobTimeR   = 1;
    size_t knobFeedback = 2;
    size_t knobBlend   = 3;
    size_t knobModRate = 4;
    size_t knobModDepth = 5;
    float  maxDelaySeconds = 0.5f;
};

class DelayEffect : public EffectBase {
public:
    explicit DelayEffect(Controls& controls, DelayConfig cfg = {})
        : EffectBase(controls), cfg_(cfg) {}

    void Init(float sampleRate);
    void UpdateParameters() override;
    void Process(const float* in, float* out, size_t size) override;
    void UpdateUI() override;

private:
    DelayConfig  cfg_;
    float sampleRate_  = 48000.f;
    float feedback_    = 0.4f;
    float targetTimeL_ = 0.25f;
    float targetTimeR_ = 0.25f;
    float currentTimeL_ = 0.25f;
    float currentTimeR_ = 0.25f;
    float dryMix_  = 0.5f;
    float wetMix_  = 0.5f;
    float modRate_ = 0.5f;
    float modDepth_ = 0.005f;

    std::vector<float> bufL_, bufR_;
    int writeL_ = 0, writeR_ = 0;

    drouAudioLib::Lfo lfo_;
    DcBlock           dcL_, dcR_;

    float processSample(float in, bool isLeft);
    float getModulatedTime(bool isLeft);
};

} // namespace droubox
