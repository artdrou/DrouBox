#include "onOffOnSwitches.h"

// Initialize the GPIO pins
void OnOffOnSwitchManager::Init(dsy_gpio_pin leftPin, dsy_gpio_pin rightPin)
{
    dsy_gpio_pin hwPins[2] = {leftPin, rightPin};
    for(int i = 0; i < 2; i++)
    {
        pins[i].pin  = hwPins[i];
        pins[i].mode = DSY_GPIO_MODE_INPUT;
        pins[i].pull = DSY_GPIO_PULLUP;
        dsy_gpio_init(&pins[i]);
    }
}

// Update state from hardware
void OnOffOnSwitchManager::Update()
{
    bool left  = !dsy_gpio_read(&pins[0]);
    bool right = !dsy_gpio_read(&pins[1]);

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
