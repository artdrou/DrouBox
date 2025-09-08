#pragma once
#include "footSwitches.h"
#include <cstdint>
#include <cmath>

class FootswitchPair {
public:
    FootswitchPair(FootswitchManager* a, FootswitchManager* b);

    void Update(); // call every loop

    // Single footswitch events
    bool ATap(int maxTapMs = 1000);
    bool BTap(int maxTapMs = 1000);
    bool AHeld(float seconds = 1.f);
    bool BHeld(float seconds = 1.f);

    // Pair events
    bool BothTapped(int maxTapMs = 1000, uint32_t tapToleranceMs = 250);
    bool BothHeldForTrigger(float seconds = 1.f, uint32_t holdToleranceMs = 250);

private:
    FootswitchManager* a_;
    FootswitchManager* b_;

    uint32_t tapLockMs_ = 500;
    uint32_t holdLockMs_ = 500;

    uint32_t lockoutUntilMs_ = 0;
    uint32_t tapToleranceMs_ = 250;

    bool bothHoldTriggered_ = false;
};
