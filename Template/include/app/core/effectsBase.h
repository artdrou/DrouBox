#pragma once

// Forward declarations
class ControlMapper;
class Controls;

class EffectBase {
public:
    virtual void UpdateParameters(const ControlMapper& mapper) = 0;
    virtual void Process(const float* in, float* out, size_t size) = 0;
    virtual void UpdateUI(Controls& controls) {}

    virtual ~EffectBase() {}
};