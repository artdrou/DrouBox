#include "controls.h"
#include "controlConfig.h"

void Controls::Init(DaisySeed& hw)
{
    hwPtr = &hw;

    // Init Knobs
    knobs.Init(hw, ControlsConfig::knobs, ControlsConfig::numKnobs);

    // Init LEDs
    for(size_t i = 0; i < ControlsConfig::numLeds; i++)
    {
        leds[i].Init(ControlsConfig::leds[i]);
    }

    // Init Footswitches
    for(size_t i = 0; i < ControlsConfig::numFootswitches; i++)
    {
        footswitches[i].Init(ControlsConfig::footswitches[i]);
    }

    // Init On-Off-On Switches
    for(size_t i = 0; i < ControlsConfig::numSwitches; i++)
    {
        dsy_gpio_pin pinArray[ControlsConfig::switches[i].size()];
        for(size_t j = 0; j < ControlsConfig::switches[i].size(); j++)
            pinArray[j] = hw.GetPin(ControlsConfig::switches[i].pins[j]);
    
        switches[i].Init(pinArray, ControlsConfig::switches[i].size());
    }

    // Init Dips Switches
    for(size_t i = 0; i < ControlsConfig::numDips; i++)
    {
        dsy_gpio_pin pinArray[ControlsConfig::dips[i].size()];
        for(size_t j = 0; j < ControlsConfig::dips[i].size(); j++)
            pinArray[j] = hw.GetPin(ControlsConfig::dips[i].pins[j]);

        dips[i].Init(pinArray, ControlsConfig::dips[i].size());
    }
}
