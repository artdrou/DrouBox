#pragma once
#include "daisy_seed.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;
using namespace daisy::seed;

template <size_t N>
struct PinList
{
    int pins[N];

    constexpr size_t size() const { return N; }
};

struct ControlsConfig
{
    // Knobs
    static constexpr Pin knobs[] = {
        A1,
        A2,
        A3,
        A4,
        A5,
        A6,
    };
    static constexpr size_t numKnobs = sizeof(knobs) / sizeof(knobs[0]);

    // LEDs
    static constexpr Pin leds[] = {
        A7,
        A8
    };
    static constexpr size_t numLeds = sizeof(leds) / sizeof(leds[0]); 

    // FOOTSWITCHS
    static constexpr Pin footswitches[] = {
        D25,
        D26
    };
    static constexpr size_t numFootswitches = sizeof(footswitches) / sizeof(footswitches[0]);

    // ON-OFF-ON SWITCHES
    static constexpr PinList<2> switches[] = {
        {{14, 13}},
        {{7, 10}},
        {{2, 4}}
    };
    static constexpr size_t numSwitches = sizeof(switches) / sizeof(switches[0]);

    // DIPS SWITCHES
    static constexpr PinList<4> dips[] = {
        {{1, 3, 5, 6}}
    };
    static constexpr size_t numDips = sizeof(dips) / sizeof(dips[0]);
};
