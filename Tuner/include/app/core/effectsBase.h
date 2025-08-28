#pragma once

// Forward declarations
class ControlMapper;
class Controls;

class EffectBase {
public:

    EffectBase(Controls& controlsRef) : controls_(controlsRef) {}
    virtual void UpdateParameters() = 0;
    virtual void Process(const float* in, float* out, size_t size) = 0;
    virtual void UpdateUI() {}

    virtual ~EffectBase() {}
protected:
    Controls& controls_;
};