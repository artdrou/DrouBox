#include "compressor.h"
#include "mapping.h"
#include <algorithm>

namespace droubox {

void Compressor::Init(float sampleRate) {
    sampleRate_ = sampleRate;
    envelope_   = 0.f;
    rmsWindow_  = 0.f;
    updateCoeffs();
}

void Compressor::SetThreshold(float thresh_dB) {
    threshold_ = dBToLinear(thresh_dB);
}

void Compressor::SetRatio(float ratio) {
    ratio_ = ratio;
}

void Compressor::SetAttack(float attack_ms) {
    attackCoeff_ = std::exp(-1.f / ((attack_ms / 1000.f) * sampleRate_));
}

void Compressor::SetRelease(float release_ms) {
    releaseCoeff_ = std::exp(-1.f / ((release_ms / 1000.f) * sampleRate_));
}

void Compressor::SetMakeupGain(float gain_dB) {
    makeupGain_ = dBToLinear(gain_dB);
}

void Compressor::SetBlend(float blend) {
    blend_ = std::clamp(blend, 0.f, 1.f);
}

void Compressor::SetKneeWidth(float kneeWidth_dB) {
    kneeWidth_ = kneeWidth_dB;
}

void Compressor::SetUseRMS(bool enable) {
    useRMS_    = enable;
    rmsWindow_ = 0.f;
}

void Compressor::updateCoeffs() {
    SetAttack(10.f);
    SetRelease(100.f);
}

void Compressor::UpdateFromKnobs(float sustainKnob, float attackKnob, float blendKnob) {
    SetThreshold(linMap(sustainKnob, 0.f, 1.f, 0.f, -40.f));
    SetRatio(linMap(sustainKnob, 0.f, 1.f, 1.f, 8.f));
    SetAttack(linMap(attackKnob, 0.f, 1.f, 5.f, 200.f));
    SetBlend(blendKnob);
}

float Compressor::calcEnvelope(float level) {
    float detector = level;
    if (useRMS_) {
        rmsWindow_ = 0.99f * rmsWindow_ + 0.01f * (level * level);
        detector   = std::sqrt(rmsWindow_ + 1e-8f);
    }
    if (detector > envelope_)
        envelope_ = attackCoeff_  * (envelope_ - detector) + detector;
    else
        envelope_ = releaseCoeff_ * (envelope_ - detector) + detector;
    return envelope_;
}

float Compressor::calcGainReduction(float env) {
    float dBIn  = linearToDB(env);
    float dBThr = linearToDB(threshold_);
    float dBGR  = 0.f;

    if (kneeWidth_ > 0.f) {
        float half  = kneeWidth_ / 2.f;
        float lower = dBThr - half;
        float upper = dBThr + half;
        if (dBIn > lower && dBIn < upper) {
            float x = (dBIn - lower) / kneeWidth_;
            float interpRatio = 1.f + (ratio_ - 1.f) * x * x;
            dBGR = (dBThr + (dBIn - dBThr) / interpRatio) - dBIn;
        } else if (dBIn >= upper) {
            dBGR = (dBThr + (dBIn - dBThr) / ratio_) - dBIn;
        }
    } else if (env > threshold_) {
        dBGR = (dBThr + (dBIn - dBThr) / ratio_) - dBIn;
    }
    return dBToLinear(dBGR);
}

float Compressor::Process(float in) {
    float level  = std::fabs(in);
    if (level < 1e-6f) level = 1e-6f;
    float env    = calcEnvelope(level);
    float gain   = calcGainReduction(env);
    float wet    = in * gain * makeupGain_;
    return (1.f - blend_) * in + blend_ * wet;
}

} // namespace droubox
