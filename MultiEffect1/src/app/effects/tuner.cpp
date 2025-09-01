#include <vector>
#include <algorithm>
#include <complex>
#include <cmath>
#include "tuner.h"
#include "controls.h"

void Tuner::UpdateParameters() {
    controls_.Update();
    params_.bypass = controls_.GetFootswitch(0).GetState();
};

void Tuner::Process(const float* in, float* out, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        out[i] = in[i];
    }
    PushBlock(in, size);   
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
    size_t start = writeIndex_;
    for (size_t i = 0; i < N; i++) {
        ordered[i] = buffer_[(start + i) % N];
    }
    return ordered;
}

void Tuner::UpdateUI() {
    sampleRate_ = controls_.GetHwPtr()->AudioSampleRate();
    if (!params_.bypass) {
        YinPitchDetection();
        UpdateTuningLeds();
    }
    else {
        controls_.GetLed(0).Set(false);
        controls_.GetLed(1).Set(false);
    }
    
}

void Tuner::YinPitchDetection() {
    auto window = GetBufferOrdered();
    yin_.SetSampleRate(controls_.GetHwPtr()->AudioSampleRate());
    yin_.SetDecimation(4);
    yin_.SetMinFreq(75);
    yin_.SetMaxFreq(15000);
    yin_.SetThreshold(0.15f);
    float detected = yin_.DetectPitch(window);
    if (detected > 0 && frequency_ > 0)
        frequency_ = (1 - smoothingFactor_) * frequency_ + smoothingFactor_ * detected;
    else if (detected > 0 && frequency_ == -1.f)
        frequency_ = detected;
    else
        frequency_ = -1.f;

    // controls_.GetHwPtr()->PrintLine("frequency candidate: %d.%02d", (int)frequency_, (int)((frequency_ - (int)frequency_) * 100));
}

void Tuner::UpdateTuningLeds() {
    float closestDiff = std::numeric_limits<float>::max();
    closestString_ = -1;
    for (size_t i = 0; i < stringFreqs_.size(); ++i) {
        diff_ = std::abs(frequency_ - stringFreqs_[i]);
        if (diff_ < closestDiff) {
            closestDiff = diff_;
            closestString_ = static_cast<int>(i);
        }
    }
    float target = stringFreqs_[closestString_];
    diff_ = frequency_ - target;
    float absDiff = std::abs(diff_);

    // smaller diff => closer to 1.0f
    float brightness = 1.0f - (absDiff / (float)maxDeviationHz_);
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
