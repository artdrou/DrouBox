#include "knobs.h"
#include <cmath>

void KnobsManager::Init(DaisySeed& hwRef, dsy_gpio_pin knobPins[], size_t n) {
    if(n > MAX_KNOBS) n = MAX_KNOBS;
    numKnobs = n;
    hwPtr = &hwRef;

    for(size_t i = 0; i < numKnobs; i++) {
        adcConfig[i].InitSingle(knobPins[i]);
        values[i] = 0;
        lastValues[i] = 0;
    }

    hwPtr->adc.Init(adcConfig, numKnobs);
    hwPtr->adc.Start();  // Start ADC once
}

void KnobsManager::Update() {
    if(!hwPtr) return;
    for(size_t i = 0; i < numKnobs; i++) {
        float current = hwPtr->adc.GetFloat(i);
        lastValues[i] = values[i];  // store old smoothed value
        float smoothed = alpha * current + (1.0f - alpha) * values[i];
        values[i] = smoothed;
    }
}

float KnobsManager::GetValue(size_t index) {
    if(index >= numKnobs) {
        return 0.0f;
    }
    return values[index];
}

bool KnobsManager::HasChanged(size_t index, float threshold) {
    if(index >= numKnobs) return false;
    return std::fabs(values[index] - lastValues[index]) > threshold;
}
