// controls.h
#ifndef CONTROLS_H
#define CONTROLS_H

#include "daisy_seed.h"
#include "daisy_petal.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;
using namespace daisy::seed;

// --- Global Hardware Objects ---
extern DaisySeed hw;



// Control Parameters
extern float onOff;
extern float knob1;
extern float knob2;
extern float knob3;
extern float knob4;
extern float knob5;
extern float knob6;

// --- ADC Channels ---
enum AdcChannel
{
    KNOB_1,
    KNOB_2,
    KNOB_3,
    KNOB_4,
    KNOB_5,
    KNOB_6,
    NUM_ADC_CHANNELS
};

// --- Setup Functions ---
void SetupKnobs();

// --- Update Functions ---
void updateKnobs();

// --- Utils functions ---
float smoothKnobValue(float currentValue, float lastSmoothedValue, float alpha);


#endif // CONTROLS_H
