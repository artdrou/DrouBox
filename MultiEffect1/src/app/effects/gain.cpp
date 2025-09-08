#include "gain.h"
#include "controls.h"

void Gain::UpdateParameters() {
    params_.gain = controls_.GetKnobs().GetValue(gainConfig_.knobGain);
};

void Gain::Process(const float* in, float* out, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        out[i] = params_.gain*in[i];
    }  
}


void Gain::UpdateUI() {
    if (controlsActive) {
        if (effectActive) {
            controls_.GetLed(0).Set(true);
        }
        else {
            controls_.GetLed(0).Set(false);
            controls_.GetLed(1).Set(false);
        }
    }
    else {
        controls_.GetLed(0).Set(true);
        controls_.GetLed(1).Set(true);
    }
}

