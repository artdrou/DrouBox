#pragma once
#include <cstddef>
#include "effectsBase.h"
#include "effectConfig.h"


class Gain : public EffectBase {
public:
    struct effectParams {
        float gain;
    };

    Gain(Controls& controlsRef)
        : EffectBase(controlsRef) {
            SetUpdateRateMs(10.0f);
        }

    void UpdateParameters();
    void Process(const float* in, float* out, size_t size) override;
    void UpdateUI() override;

private:
    effectParams params_;
    GainConfig gainConfig_;
};