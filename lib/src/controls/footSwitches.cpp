#include "footSwitches.h"

void FootswitchManager::Init(daisy::Pin pin) {
    footswitch.Init(pin, 1000);
}

void FootswitchManager::Update() {
    footswitch.Debounce();
    if (this->Pressed()) {
        state = !state;
    }
}

bool FootswitchManager::Pressed() {
    return footswitch.RisingEdge();
}

bool FootswitchManager::Released() {
    return footswitch.FallingEdge();
}

float FootswitchManager::Held() {
    return footswitch.TimeHeldMs();
}

bool FootswitchManager::GetState() {
    return state;
}