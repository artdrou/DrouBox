#pragma once
#include "daisy_seed.h"

using namespace daisy;

class OnOffOnSwitchManager
{
public:
    enum class State { LEFT, OFF, RIGHT };

    void Init(dsy_gpio_pin leftPin, dsy_gpio_pin rightPin);

    void Update();             // Read hardware
    bool HasChanged();         // Returns true if state changed since last Update
    State GetState() const;    // Current state
    State GetLastState() const;// Previous state
    const char* ToString(State s) const;

private:
    dsy_gpio pins[2]; // Left + Right contacts
    State state      = State::OFF;
    State lastState  = State::OFF;
};
