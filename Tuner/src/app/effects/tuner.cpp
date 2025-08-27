#include <vector>
#include <algorithm>
#include <complex>
#include <cmath>
#include "tuner.h"
#include "controlMapper.h"
#include "controls.h"

void Tuner::UpdateParameters() {
    params_ = mapper_.MapTuner();
};

void Tuner::Process(const float* in, float* out, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        out[i] = in[i];
    }
    PushBlock(in, size);
    if (!params_.bypass) {
        static int callbackCount = 0;
        callbackCount++;
        if(callbackCount % 50 == 0){
            DetectPitch();
        }
    }    
}

void Tuner::PushBlock(const float* input, size_t size) {
    for (size_t i = 0; i < size; i++) {
        buffer_[writeIndex_] = input[i];
        writeIndex_ = (writeIndex_ + 1) % buffer_.size();
    }
}

std::vector<float> Tuner::GetBufferOrdered() const {
    std::vector<float> ordered(buffer_.size());
    size_t N = buffer_.size();
    size_t start = writeIndex_; // oldest sample
    for (size_t i = 0; i < N; i++) {
        ordered[i] = buffer_[(start + i) % N];
    }
    return ordered;
}

void Tuner::UpdateUI() {
    Controls& controls_ = mapper_.GetControls();
    frequency = frequency_;
    if (!params_.bypass) {
        UpdateTuningDifference();
        UpdateTuningLeds();
    }
    else {
        controls_.GetLed(0).Set(false);
        controls_.GetLed(1).Set(false);
    }
    
}

void Tuner::DetectPitch() {
    Controls& controls_ = mapper_.GetControls();


    auto window = GetBufferOrdered();
    frequency_ = CMNDFPitchDetection(
        window, controls_.GetSampleRate()
    );

    
    controls_.GetHwPtr()->PrintLine(
    "frequency candidate: %d.%02d", 
    (int)frequency, 
    (int)((frequency - (int)frequency) * 100));
}

void Tuner::UpdateTuningDifference() {
    float closestDiff = std::numeric_limits<float>::max();
    closestString_ = -1;
    for (size_t i = 0; i < stringFreqs_.size(); ++i) {
        diff_ = std::abs(frequency - stringFreqs_[i]);
        if (diff_ < closestDiff) {
            closestDiff = diff_;
            closestString_ = static_cast<int>(i);
        }
    }
    float target = stringFreqs_[closestString_];
    diff_ = frequency - target;

}

void Tuner::UpdateTuningLeds() {
    Controls& controls_ = mapper_.GetControls();
    float absDiff = std::abs(diff_);

    // smaller diff => closer to 1.0f
    float brightness = 1.0f - (absDiff / maxDeviationHz_);
    if (brightness < 0.0f) brightness = 0.0f;
    if (brightness > 1.0f) brightness = 1.0f;

    bool tooLow  = diff_ < -toleranceHz_;
    bool tooHigh = diff_ >  toleranceHz_;
    bool inTune  = !tooLow && !tooHigh;

    // If in tune, light both with brightness depending on how close we are
    if (inTune) {
        controls_.GetLed(0).Set(brightness);
        controls_.GetLed(1).Set(brightness);
    }
    else if (tooLow) {
        controls_.GetLed(0).Set(brightness);
        controls_.GetLed(1).Set(0.0f);
    }
    else if (tooHigh) {
        controls_.GetLed(0).Set(0.0f);
        controls_.GetLed(1).Set(brightness);
    }
}
