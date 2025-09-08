#pragma once
#include "effectsBase.h"
#include "disto.h"
#include "utils.h"

class Overdrive : public EffectBase {
public:
    struct effectParams {
        bool bypass = false;
        int type = 1;
        float drive = 0.5f;
        float level = 0.7f;
        float tone = 0.5f;
    };

    Overdrive() { SetUpdateRateMs(5.0f); }

    void SetParams(const effectParams& p) { params_ = p; }
    effectParams& GetParams() { return params_; }

    void Process(const float* in, float* out, size_t size) override {
        if (params_.bypass) {
            std::copy(in, in + size, out);
            return;
        }
        for (size_t i = 0; i < size; i++) {
            float distorted = processDistortionFromDipsValue(
                params_.type, in[i], params_.drive, params_.level);
            out[i] = ApplyTone(distorted);
        }
    }

    void UpdateParameters() override {}
    void UpdateUI() override {}

private:
    effectParams params_;

    // Simple tone filter (placeholder)
    float ApplyTone(float in) {
        return in * params_.tone; // TODO: proper EQ
    }
};
