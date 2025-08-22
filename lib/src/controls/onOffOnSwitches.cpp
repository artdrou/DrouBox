#include "onOffOnSwitches.h"

// Initialize the GPIO pins
void OnOffOnSwitchManager::Init(dsy_gpio_pin pins[], size_t n)
{
    nSwPins = n;
    swPins = new dsy_gpio[nSwPins];
    for(int i = 0; i < nSwPins; i++)
    {
        swPins[i].pin  = pins[i];
        swPins[i].mode = DSY_GPIO_MODE_INPUT;
        swPins[i].pull = DSY_GPIO_PULLUP;
        dsy_gpio_init(&swPins[i]);
    }
}

// Update state from hardware
void OnOffOnSwitchManager::Update()
{
    bool left  = !dsy_gpio_read(&swPins[0]);
    bool right = !dsy_gpio_read(&swPins[1]);

    if(left && !right)
        state = State::LEFT;
    else if(!left && right)
        state = State::RIGHT;
    else
        state = State::OFF;
}

// Detect change
bool OnOffOnSwitchManager::HasChanged()
{
    Update();
    if(state != lastState)
    {
        lastState = state;
        return true;
    }
    return false;
}

OnOffOnSwitchManager::State OnOffOnSwitchManager::GetState() const
{
    return state;
}

OnOffOnSwitchManager::State OnOffOnSwitchManager::GetLastState() const
{
    return lastState;
}

const char* OnOffOnSwitchManager::ToString(State s) const
{
    switch(s)
    {
        case State::LEFT:  return "LEFT";
        case State::RIGHT: return "RIGHT";
        case State::OFF:   return "OFF";
        default:           return "UNKNOWN";
    }
}
