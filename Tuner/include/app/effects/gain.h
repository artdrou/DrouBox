#pragma once
#include <cstddef>
#include "effectsBase.h"

class Gain : public EffectBase {
public:
    struct effectParams {
        float gain = 1.0f;
        bool bypass = false;
    };

    Gain() {}

    void UpdateParameters(const ControlMapper& mapper);
    void Process(const float* in, float* out, size_t size) override;
    void UpdateUI(Controls& controls) override;

    effectParams GetParams() const;

private:
     effectParams params_;
};