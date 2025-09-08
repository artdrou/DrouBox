#pragma once
#include "footSwitches.h"

class FootswitchPair {
public:
    FootswitchPair(FootswitchManager* a, FootswitchManager* b);

    void Update();

    // Pair actions
    bool BothTapped(int maxTapTimeMs, int toleranceMs = 200);
    bool BothHeldForTrigger(float seconds, int toleranceMs = 200);

    // Individual access
    bool ATap(int maxTapTimeMs);
    bool BTap(int maxTapTimeMs);
    bool AHeld(float seconds);
    bool BHeld(float seconds);

private:
    FootswitchManager* a_;
    FootswitchManager* b_;

    uint32_t lockoutUntilMs_ = 0;
    uint32_t lockoutDurationMs_ = 300;

    bool suppressATap_ = false;
    bool suppressBTap_ = false;
};
