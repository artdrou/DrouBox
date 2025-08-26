#pragma once

// Forward declarations
class ControlMapper;
class Controls;

class EffectBase {
public:

    EffectBase(ControlMapper& mapperRef) : mapper_(mapperRef) {}
    virtual void UpdateParameters() = 0;
    virtual void Process(const float* in, float* out, size_t size) = 0;
    virtual void UpdateUI() {}

    virtual ~EffectBase() {}
protected:
    ControlMapper& mapper_;
};