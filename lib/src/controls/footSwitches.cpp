#include "footSwitches.h"

void FootswitchManager::Init(daisy::Pin pin) {
    footswitch.Init(pin, 1000);
}

void FootswitchManager::Update() {
    footswitch.Debounce();
    if (JustPressed()) {
        state_ = !state_;
    } else if (footswitch.Pressed()) {
        holdMs_ = Held();
    }
}

bool FootswitchManager::JustPressed() {
    return footswitch.RisingEdge();
}

bool FootswitchManager::IsPressed() {
    return footswitch.Pressed();
}

bool FootswitchManager::IsTap(int maxTapTimeMs) {
    if (Released()) {
        return holdMs_ <= maxTapTimeMs;
    }
    return false;
}

bool FootswitchManager::Released() {
    return footswitch.FallingEdge();
}

float FootswitchManager::Held() {
    return footswitch.TimeHeldMs();
}

bool FootswitchManager::HeldFor(float seconds) {
    return Held() >= (seconds * 1000.0f);
}

bool FootswitchManager::IsHeldForTrigger(float seconds) {
    bool event = false;

    if (IsPressed()) {
        if (!holdTriggered_ && Held() >= seconds * 1000.0f) {
            holdTriggered_ = true;
            event = true; // return true only **once** when threshold is reached
        }
    } else {
        holdTriggered_ = false; // reset when released
    }

    return event;
}

bool FootswitchManager::GetState() {
    return state_;
}