#pragma once
#include "daisy_seed.h"
#include <array>

using namespace daisy;

// ── DrouBox hardware pin map ───────────────────────────────────────────────
// All firmwares share the same physical enclosure, so one config fits all.
// ──────────────────────────────────────────────────────────────────────────

namespace ControlsConfig {

// Knobs (6 ADC channels)
inline constexpr size_t numKnobs = 6;
inline const Pin knobs[numKnobs] = {
    seed::A1, seed::A2, seed::A3,
    seed::A4, seed::A5, seed::A6
};

// Footswitches (2 momentary normally-open, active-low)
inline constexpr size_t numFootswitches = 2;
inline const Pin footswitches[numFootswitches] = { seed::D25, seed::D26 };

// LEDs (2 PWM)
inline constexpr size_t numLeds = 2;
inline const Pin leds[numLeds] = { seed::A7, seed::A8 };

// On-Off-On 3-position switches (3 switches, 2 pins each)
inline constexpr size_t numSwitches = 3;

struct SwitchPins {
    Pin pins[2];
    constexpr size_t size() const { return 2; }
};

inline const SwitchPins switches[numSwitches] = {
    { { seed::D14, seed::D13 } },
    { { seed::D7,  seed::D10 } },
    { { seed::D2,  seed::D4  } },
};

// DIP switches (1 bank of 4)
inline constexpr size_t numDips = 1;

struct DipPins {
    Pin pins[4];
    constexpr size_t size() const { return 4; }
};

inline const DipPins dips[numDips] = {
    { { seed::D1, seed::D3, seed::D5, seed::D6 } }
};

} // namespace ControlsConfig
