#pragma once

#include "effectsBase.h"
#include "controls.h"
#include <vector>

class EffectManager {
public:
    EffectManager(Controls& controlsRef) : controls_(controlsRef), activeIndex_(0) {}

    void SetBlockSize(size_t blockSize);
    void AddEffect(EffectBase* effect);
    void NextEffect();
    void PreviousEffect();
    EffectBase* GetActiveEffect();

    void UpdateParameters();
    void Process(const float* in, float* out, size_t size);
    void UpdateUI();
    void Bypass(const float* in, float* out, size_t size);
    float GetActiveUpdateRateMs() const;
    void ProcessControlGestures();

private:
    std::vector<EffectBase*> effects_;
    Controls& controls_;
    size_t activeIndex_;
    bool globalBypass_ = false;
    // Buffers for ping-pong chain processing
    std::vector<float> bufferA_;
    std::vector<float> bufferB_;
};
