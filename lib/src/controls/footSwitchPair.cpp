#include "footSwitchPair.h"
#include <cmath> // std::abs

FootswitchPair::FootswitchPair(FootswitchManager* a, FootswitchManager* b)
    : a_(a), b_(b) {}

void FootswitchPair::Update() {
    a_->Update();
    b_->Update();
}

bool FootswitchPair::ATap(int maxTapTimeMs) {
    if (daisy::System::GetNow() < lockoutUntilMs_)
        return false; // temporarily blocked
    if (suppressATap_) { suppressATap_ = false; return false; }
    return a_->IsTap(maxTapTimeMs);
}

bool FootswitchPair::BTap(int maxTapTimeMs) {
    if (daisy::System::GetNow() < lockoutUntilMs_)
        return false;
    if (suppressBTap_) { suppressBTap_ = false; return false; }
    return b_->IsTap(maxTapTimeMs);
}

bool FootswitchPair::AHeld(float seconds) {
    if (daisy::System::GetNow() < lockoutUntilMs_)
        return false;
    return a_->IsHeldForTrigger(seconds);
}

bool FootswitchPair::BHeld(float seconds) {
    if (daisy::System::GetNow() < lockoutUntilMs_)
        return false;
    return b_->IsHeldForTrigger(seconds);
}

bool FootswitchPair::BothTapped(int maxTapTimeMs, int toleranceMs) {
    if (daisy::System::GetNow() < lockoutUntilMs_)
        return false; // currently locked

    bool aTap = a_->IsTap(maxTapTimeMs);
    bool bTap = b_->IsTap(maxTapTimeMs);

    uint32_t lastA = a_->GetLastTapTimeMs();
    uint32_t lastB = b_->GetLastTapTimeMs();

    auto within = [&](uint32_t x, uint32_t y) {
        return (x && y) && (std::abs((int)x - (int)y) <= toleranceMs);
    };

    if ((aTap && bTap) || (aTap && within(lastA, lastB)) || (bTap && within(lastA, lastB))) {
        lockoutUntilMs_ = daisy::System::GetNow() + lockoutDurationMs_;
        suppressATap_ = true;
        suppressBTap_ = true;
        return true;
    }

    return false;
}

bool FootswitchPair::BothHeldForTrigger(float seconds, int toleranceMs) {
    static bool triggered = false;

    if (daisy::System::GetNow() < lockoutUntilMs_)
        return false; // currently locked

    bool aHeld = a_->HeldFor(seconds);
    bool bHeld = b_->HeldFor(seconds);

    uint32_t aPressT = a_->GetLastPressTimeMs();
    uint32_t bPressT = b_->GetLastPressTimeMs();

    auto within = [&](uint32_t x, uint32_t y) {
        return (x && y) && (std::abs((int)x - (int)y) <= toleranceMs);
    };

    if (aHeld && bHeld && within(aPressT, bPressT)) {
        if (!triggered) {
            triggered = true;
            lockoutUntilMs_ = daisy::System::GetNow() + lockoutDurationMs_;
            return true;
        }
    } else {
        triggered = false;
    }

    return false;
}
