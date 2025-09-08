#pragma once

#include "daisy_seed.h"
#include "daisy_petal.h"
#include <vector>

class FootswitchManager {
public:
    void Init(daisy::Pin pin);
    void Update();
    bool JustPressed();
    bool IsPressed();
    bool IsTap(int maxTapTimeMs);
    bool Released();
    float Held();
    bool HeldFor(float seconds);
    bool IsHeldForTrigger(float seconds);
    bool GetState();
    uint32_t GetLastTapTimeMs() const { return lastTapTimeMs_; }
    uint32_t GetLastPressTimeMs() const { return lastPressTimeMs_; }
    void InvalidateTap() { lastTapTimeMs_ = 0; }
private:
    daisy::Switch footswitch;
    bool state_ = false;
    bool holdTriggered_ = false;
    float holdMs_ = 0.0f;
    uint32_t lastTapTimeMs_ = 0;
    uint32_t lastPressTimeMs_ = 0;
};