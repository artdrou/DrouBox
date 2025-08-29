#pragma once

// Forward declarations
class Controls;

class EffectBase {
public:

    EffectBase(Controls& controlsRef) : controls_(controlsRef) {}
    virtual void UpdateParameters() = 0;
    virtual void Process(const float* in, float* out, size_t size) = 0;
    virtual void UpdateUI() {}
    virtual float GetUpdateRateMs() const { return defaultUpdateRateMs_; }
    virtual void SetUpdateRateMs(float ms) { updateRateMs_ = ms; }

    virtual ~EffectBase() {}
protected:
    Controls& controls_;
    float updateRateMs_ = 10.0f;
    static constexpr float defaultUpdateRateMs_ = 10.0f;
};