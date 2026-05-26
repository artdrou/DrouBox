#include "footSwitchPair.h"
#include "daisy_seed.h" // for System::GetNow()

FootswitchPair::FootswitchPair(FootswitchManager* a, FootswitchManager* b)
    : a_(a), b_(b) { }

void FootswitchPair::Update() {
    a_->Update();
    b_->Update();
}

// ---------------- Pair events ----------------
bool FootswitchPair::BothTapped(int maxTapMs, uint32_t tapToleranceMs) {
    if (daisy::System::GetNow() < lockoutUntilMs_) return false;

    bool aTap = a_->IsTap(maxTapMs);
    bool bTap = b_->IsTap(maxTapMs);

    uint32_t lastATap = a_->GetLastTapTimeMs();
    uint32_t lastBTap = b_->GetLastTapTimeMs();

    auto within = [&](uint32_t x, uint32_t y) {
        return x != kNeverPressedMs && y != kNeverPressedMs
            && (std::abs((int)x - (int)y) <= (int)tapToleranceMs);
    };

    if ((aTap && bTap) || (aTap && within(lastATap,lastBTap)) || (bTap && within(lastATap,lastBTap))) {
        lockoutUntilMs_ = daisy::System::GetNow() + tapLockMs_;
        return true;
    }
    return false;
}

bool FootswitchPair::BothHeldForTrigger(float seconds, uint32_t holdToleranceMs) {
    bool aHeld = a_->Held() >= seconds*1000;
    bool bHeld = b_->Held() >= seconds*1000;

    uint32_t aPress = a_->GetLastPressTimeMs();
    uint32_t bPress = b_->GetLastPressTimeMs();

    auto within = [&](uint32_t x, uint32_t y) {
        return x != kNeverPressedMs && y != kNeverPressedMs
            && (std::abs((int)x - (int)y) <= (int)holdToleranceMs);
    };

    if (aHeld && bHeld && within(aPress,bPress)) {
        if (!bothHoldTriggered_) {
            bothHoldTriggered_ = true;
            lockoutUntilMs_ = daisy::System::GetNow() + holdLockMs_;
            return true;
        }
    } else {
        bothHoldTriggered_ = false;
    }
    return false;
}

// ---------------- Single events ----------------
bool FootswitchPair::ATap(int maxTapMs) {
    if (daisy::System::GetNow() < lockoutUntilMs_) return false;
    if (b_->IsPressed()) return false;
    uint32_t lastATap = a_->GetLastTapTimeMs();
    uint32_t lastBTap = b_->GetLastTapTimeMs();
    bool bReleasedTap = b_->Released() && b_->Held() <= maxTapMs;
    bool aReleasedTap = a_->Released() && a_->Held() <= maxTapMs;
    if (aReleasedTap && bReleasedTap &&
        std::abs((int)lastATap - (int)lastBTap) <= (int)tapToleranceMs_) {
        return false;
    }
    return a_->IsTap(maxTapMs);
}

bool FootswitchPair::BTap(int maxTapMs) {
    if (daisy::System::GetNow() < lockoutUntilMs_) return false;
    if (a_->IsPressed()) return false;
    uint32_t lastATap = a_->GetLastTapTimeMs();
    uint32_t lastBTap = b_->GetLastTapTimeMs();
    bool bReleasedTap = b_->Released() && b_->Held() <= maxTapMs;
    bool aReleasedTap = a_->Released() && a_->Held() <= maxTapMs;
    if (aReleasedTap && bReleasedTap &&
        std::abs((int)lastATap - (int)lastBTap) <= (int)tapToleranceMs_) {
        return false;
    }
    return b_->IsTap(maxTapMs);
}

bool FootswitchPair::AHeld(float seconds) {
    if (daisy::System::GetNow() < lockoutUntilMs_) return false;
    if (b_->IsPressed()) return false;
    return a_->IsHeldForTrigger(seconds);
}

bool FootswitchPair::BHeld(float seconds) {
    if (daisy::System::GetNow() < lockoutUntilMs_) return false;
    if (a_->IsPressed()) return false;
    return b_->IsHeldForTrigger(seconds);
}
