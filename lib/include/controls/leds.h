#pragma once
#include <cstdint>
#include "daisy_seed.h"

using namespace daisy;
using namespace daisy::seed;

class LedManager {
public:
    void Init(Pin pin);
    void Update(uint32_t now);
    void Set(bool state);
    void Set(float brightness);
    void Toggle();
    bool GetState();
    void StopBlinking();
    void BlinkFor(float durationMs, uint32_t intervalMs, uint32_t now);
    bool IsBlinking();
private:
    Led led;
    bool ledState;

    // blinking
    bool blinking;
    bool blinkState;
    int blinkCount;
    int totalBlinks;
    uint32_t blinkInterval;
    uint32_t lastBlinkTime;
};