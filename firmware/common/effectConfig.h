#pragma once
#include <cstddef>

// Per-effect knob/switch assignments — which knob index maps to which parameter.

struct GainConfig {
    size_t knobGain = 0;
};

struct TunerConfig {
    // no configurable parameters
};

struct DelayEffectConfig {
    size_t knobTimeL    = 0;
    size_t knobTimeR    = 1;
    size_t knobFeedback = 2;
    size_t knobBlend    = 3;
    size_t knobModRate  = 4;
    size_t knobModDepth = 5;
};

struct DistortionEffectConfig {
    size_t knobDrive   = 0;
    size_t knobTone    = 1;
    size_t knobLevel   = 2;
    size_t knobSustain = 3;
    size_t knobAttack  = 4;
    size_t knobBlend   = 5;
    size_t dipGroup    = 0;
};

struct LooperEffectConfig {
    size_t knobBpm = 5;
};
