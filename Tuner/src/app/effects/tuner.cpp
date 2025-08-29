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
    size_t start = writeIndex_; // oldest sample
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
    yin_.SetDecimation(16);
    yin_.SetMinFreq(50);
    yin_.SetMaxFreq(10000);
    yin_.SetThreshold(0.1f);
    frequency_ = yin_.DetectPitch(window);
    // --- Find closest string ---
    float bestDiff = std::numeric_limits<float>::max();
    const StringDetectConfig* bestCfg = nullptr;

    for (auto& cfg : stringConfigs_) {
        float diff = std::abs(frequency_ - cfg.centerFreq);
        if (diff < bestDiff) {
            bestDiff = diff;
            bestCfg = &cfg;
        }
    }

    // --- Refined pass ---
    if (bestCfg) {
        yin_.SetDecimation(bestCfg->decimation);
        yin_.SetMinFreq(bestCfg->minFreq);
        yin_.SetMaxFreq(bestCfg->maxFreq);
        frequency_ = yin_.DetectPitch(window);
    }
    else {
        frequency_ = -1.f;
    }
    


    controls_.GetHwPtr()->PrintLine(
    "frequency candidate: %d.%02d", 
    (int)frequency_, 
    (int)((frequency_ - (int)frequency_) * 100));
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

void Tuner::BuildStringConfigs() {
    stringConfigs_.clear();

    // precompute midpoints between string freqs
    std::vector<float> midpoints;
    for (size_t i = 0; i + 1 < stringFreqs_.size(); ++i) {
        midpoints.push_back(0.5f * (stringFreqs_[i] + stringFreqs_[i+1]));
    }

    for (size_t i = 0; i < stringFreqs_.size(); ++i) {
        float f = stringFreqs_[i];

        // --- band edges derived from midpoints ---
        float lower = (i == 0) ? f * 0.8f : midpoints[i-1];
        float upper = (i == stringFreqs_.size()-1) ? f * 3.f : midpoints[i];

        // --- decimation based on string index ---
        int decim;
        switch (i) {
            case 0: // E2
                decim = 2;
                break;
            case 1: // A2
                decim = 2;
                break;
            case 2: // D3
                decim = 1;
                break;
            case 3: // G3
                decim = 1;
                break;
            case 4: // B3
                decim = 1;
                break;
            case 5: // E4
                decim = 1;
                break;
            default:
                decim = 16;
                break;
        }

        // --- store config ---
        StringDetectConfig cfg { f, decim, lower, upper };
        stringConfigs_.push_back(cfg);
    }
}
