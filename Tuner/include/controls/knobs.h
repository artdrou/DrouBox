#pragma once
#include "daisy_seed.h"
using namespace daisy;

class KnobsManager {
public:
    void Init(DaisySeed& hwRef, const Pin knobPins[], size_t n);
    void Update();                     // read all knobs once per loop
    float GetValue(size_t index);      // get latest value
    bool HasChanged(size_t index, float threshold = 0.01f);

private:
    static constexpr size_t MAX_KNOBS = 6;
    AdcChannelConfig adcConfig[MAX_KNOBS];
    float values[MAX_KNOBS] = {0};
    float lastValues[MAX_KNOBS] = {0};
    float alpha = 0.5f;
    size_t numKnobs = 0;
    DaisySeed* hwPtr = nullptr;
};
