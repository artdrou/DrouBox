#pragma once

#include "effectsBase.h"
#include <vector>

class EffectManager {
public:
    EffectManager() : activeIndex_(0) {}

    void AddEffect(EffectBase* effect);
    void NextEffect();
    EffectBase* GetActiveEffect();

    void UpdateParameters();
    void Process(const float* in, float* out, size_t size);
    void UpdateUI();
    void Bypass(const float* in, float* out, size_t size);
    float GetActiveUpdateRateMs() const;

private:
    std::vector<EffectBase*> effects_;
    size_t activeIndex_;
};
