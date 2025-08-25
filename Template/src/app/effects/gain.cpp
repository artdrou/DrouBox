#include "gain.h"
#include "controlMapper.h"


void Gain::UpdateParameters(const ControlMapper& mapper) {
    params_ = mapper.MapGain();
}

void Gain::Process(const float* in, float* out, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        float out_ = params_.bypass ? in[i] : in[i] * params_.gain;
        out[i] = (out_ > 1) ? 1 : out_;
    }
    
};

void Gain::UpdateUI(Controls& controls) {
    controls.GetLed(0).Set(!params_.bypass);
};

Gain::effectParams Gain::GetParams() const { return params_; }
