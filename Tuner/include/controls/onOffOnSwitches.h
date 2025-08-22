#pragma once
#include "daisy_seed.h"

using namespace daisy;

class OnOffOnSwitchManager
{
public:
    enum class State { LEFT, OFF, RIGHT };

    void Init(dsy_gpio_pin pins[], size_t n);

    void Update();             // Read hardware
    bool HasChanged();         // Returns true if state changed since last Update
    State GetState() const;    // Current state
    State GetLastState() const;// Previous state
    const char* ToString(State s) const;

    ~OnOffOnSwitchManager() { delete[] swPins; }

private:
    dsy_gpio* swPins = nullptr;
    int nSwPins;
    State state      = State::OFF;
    State lastState  = State::OFF;
};
