#pragma once

#include "daisy_seed.h"
#include "controlConfig.h"
#include "knobs.h"
#include "leds.h"
#include "footSwitches.h"
#include "dipSwitches.h"
#include "onOffOnSwitches.h"

using namespace daisy;

class Controls
{
public:
    Controls() {}

    void Init(DaisySeed& hw);
    void Update();

    // Accessors
    KnobsManager& GetKnobs() { return knobs; }
    LedManager& GetLed(size_t i) { return leds[i]; }
    FootswitchManager& GetFootswitch(size_t i) { return footswitches[i]; }
    DipManager& GetDips(size_t i) { return dips[i]; }
    OnOffOnSwitchManager& GetSwitch(size_t i) { return switches[i]; }

    // Counts
    static constexpr size_t NumLeds() { return ControlsConfig::numLeds; }
    static constexpr size_t NumFootswitches() { return ControlsConfig::numFootswitches; }
    static constexpr size_t NumSwitches() { return ControlsConfig::numSwitches; }
    static constexpr size_t NumDips() { return ControlsConfig::numDips; }
    static constexpr size_t NumKnobs() { return ControlsConfig::numKnobs; }

private:
    DaisySeed* hwPtr;

    // Managers
    KnobsManager knobs;
    
    // Multiple instances
    LedManager leds[ControlsConfig::numLeds];
    FootswitchManager footswitches[ControlsConfig::numFootswitches];
    OnOffOnSwitchManager switches[ControlsConfig::numSwitches];
    DipManager dips[ControlsConfig::numDips];
};
