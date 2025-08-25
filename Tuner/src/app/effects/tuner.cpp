#include <vector>
#include <algorithm>
#include <complex>
#include <cmath>
#include "tuner.h"
#include "controlMapper.h"

void Tuner::UpdateParameters(const ControlMapper& mapper) {
    params_ = mapper.MapTuner();
};

void Tuner::Process(const float* in, float* out, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        out[i] = in[i];
    }
}

void Tuner::UpdateUI(Controls& controls) {
    if (!params_.bypass) {
        UpdateTuningDifference(controls);
        UpdateTuningLeds(controls);
    }
    
}

void Tuner::FindFrequency() {
    std::vector<float> buffer(in, in + size);
    auto fftResult = computeFFT(buffer);

    size_t peakIndex = 0;
    float maxMag = 0.0f;
    for (size_t i = 0; i < fftResult.size() / 2; ++i) {
        float mag = std::abs(fftResult[i]);
        if (mag > maxMag) {
            maxMag = mag;
            peakIndex = i;
        }
    }

    lastPeakIndex_ = peakIndex;
    fftSize_       = fftResult.size();
    if (closestString_ < 0 && lastPeakIndex_ == 0) return;

    // Recompute frequency using the real sample rate
    float freq = lastPeakIndex_ * params_.sampleRate / fftSize_;

    smoothedFreq_ = smoothingFactor_ * freq + (1.0f - smoothingFactor_) * smoothedFreq_;
    params_.smoothedFreq = (int)smoothedFreq_;
}

void Tuner::UpdateTuningDifference(Controls& controls) {
    float closestDiff = std::numeric_limits<float>::max();
    closestString_ = -1;
    for (size_t i = 0; i < stringFreqs_.size(); ++i) {
        diff_ = std::abs(params_.smoothedFreq - stringFreqs_[i]);
        if (diff_ < closestDiff) {
            closestDiff = diff_;
            closestString_ = static_cast<int>(i);
        }
    }
    float target = stringFreqs_[closestString_];
    diff_ = params_.smoothedFreq - target;
    controls.GetHwPtr()->PrintLine("Peak frequency: %d Hz (Target: %d Hz)", (int)params_.smoothedFreq, (int)target);
}

void Tuner::UpdateTuningLeds(Controls& controls) {
    bool inTune = std::abs(diff_) <= toleranceHz_;
    bool tooLow = diff_ < -toleranceHz_;
    bool tooHigh = diff_ > toleranceHz_;
    controls.GetLed(0).Set(inTune || tooLow);
    controls.GetLed(1).Set(inTune || tooHigh);
}

Tuner::effectParams Tuner::GetParams() const { return params_; }
