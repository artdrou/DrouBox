#include "controls.h"
#include "daisy_seed.h"
#include "daisy_petal.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;
using namespace daisy::seed;

// Hardware instance
extern DaisySeed hw;

// KNOBS --------------------------------------------------------------------------------------------------
// Control variables
float onOff;
float knob1;
float knob2;
float knob3;
float knob4;
float knob5;
float knob6;

float lastValueKnob1;
float lastValueKnob2;
float lastValueKnob3;
float lastValueKnob4;
float lastValueKnob5;
float lastValueKnob6;

void SetupKnobs() {
    // ADC channels for knobs
    AdcChannelConfig adc_config[NUM_ADC_CHANNELS];
    
    // Initialize ADC channels for knobs
    adc_config[KNOB_1].InitSingle(A1);
    adc_config[KNOB_2].InitSingle(A2);
    adc_config[KNOB_3].InitSingle(A3);
    adc_config[KNOB_4].InitSingle(A4);
    adc_config[KNOB_5].InitSingle(A5);
    adc_config[KNOB_6].InitSingle(A6);
    
    // Initialize the ADC peripheral
    hw.adc.Init(adc_config, NUM_ADC_CHANNELS);
    hw.adc.Start(); // Start ADC conversion
}


void updateKnobs()
{
    // Read ADC values (normalized to 0-1)
    lastValueKnob1 = knob1;
    lastValueKnob2 = knob2;
    lastValueKnob3 = knob3;
    lastValueKnob4 = knob4;
    lastValueKnob5 = knob5;
    lastValueKnob6 = knob6;
    // Read ADC values (normalized to 0-1)
    float smoothing = 0.5f;
    knob1 = smoothKnobValue(hw.adc.GetFloat(KNOB_1), lastValueKnob1, smoothing);
    knob2 = smoothKnobValue(hw.adc.GetFloat(KNOB_2), lastValueKnob2, smoothing);
    knob3 = smoothKnobValue(hw.adc.GetFloat(KNOB_3), lastValueKnob3, smoothing);
    knob4 = smoothKnobValue(hw.adc.GetFloat(KNOB_4), lastValueKnob4, smoothing);
    knob5 = smoothKnobValue(hw.adc.GetFloat(KNOB_5), lastValueKnob5, smoothing);
    knob6 = smoothKnobValue(hw.adc.GetFloat(KNOB_6), lastValueKnob6, smoothing);
}

float smoothKnobValue(float currentValue, float lastSmoothedValue, float alpha)
{
    // Apply low-pass filter smoothing
    lastSmoothedValue = alpha * currentValue + (1.0f - alpha) * lastSmoothedValue;
    return lastSmoothedValue;
}
