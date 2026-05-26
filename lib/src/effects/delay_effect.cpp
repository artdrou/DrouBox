#include "delay_effect.h"
#include "mapping.h"
#include "controls.h"
#include <algorithm>

namespace droubox {

void DelayEffect::Init(float sampleRate) {
    sampleRate_ = sampleRate;
    int bufSize = static_cast<int>(sampleRate * cfg_.maxDelaySeconds);
    bufL_.assign(bufSize, 0.f);
    bufR_.assign(bufSize, 0.f);
    writeL_ = writeR_ = 0;
    lfo_.init(sampleRate, 0.5f, drouAudioLib::Lfo::Waveform::Sine);
}

void DelayEffect::UpdateParameters() {
    float tL       = controls_.GetKnobs().GetValue(cfg_.knobTimeL);
    float tR       = controls_.GetKnobs().GetValue(cfg_.knobTimeR);
    float feedback = controls_.GetKnobs().GetValue(cfg_.knobFeedback);
    float blend    = controls_.GetKnobs().GetValue(cfg_.knobBlend);
    float modRate  = controls_.GetKnobs().GetValue(cfg_.knobModRate);
    float modDepth = controls_.GetKnobs().GetValue(cfg_.knobModDepth);

    targetTimeL_ = linMap(tL, 0.f, 1.f, 0.001f, cfg_.maxDelaySeconds * 0.5f);
    targetTimeR_ = linMap(tR, 0.f, 1.f, 0.001f, cfg_.maxDelaySeconds * 0.5f);
    feedback_    = linMap(feedback, 0.f, 1.f, 0.f, 0.9f);
    dryMix_      = linMap(blend, 0.f, 1.f, 0.5f, 0.f);
    wetMix_      = linMap(blend, 0.f, 1.f, 0.f, 0.5f);
    modRate_     = linMap(modRate, 0.f, 1.f, 0.1f, 3.f);
    modDepth_    = linMap(modDepth, 0.f, 1.f, 0.f, 0.01f);
    lfo_.setFrequency(modRate_);
}

void DelayEffect::Process(const float* in, float* out, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        // Mono in, processed as stereo internally — write same sample to both channels
        float left  = processSample(in[i], true);
        float right = processSample(in[i], false);
        out[i] = (left + right) * 0.5f;
    }
}

float DelayEffect::getModulatedTime(bool isLeft) {
    float lfo = lfo_.getNextSample();
    float base = isLeft ? currentTimeL_ : currentTimeR_;
    float t = base + modDepth_ * lfo;
    if (t < 0.001f) t = 0.001f;
    if (t > cfg_.maxDelaySeconds) t = cfg_.maxDelaySeconds;
    return t;
}

float DelayEffect::processSample(float in, bool isLeft) {
    // Smooth delay time
    currentTimeL_ = 0.001f * targetTimeL_ + 0.999f * currentTimeL_;
    currentTimeR_ = 0.001f * targetTimeR_ + 0.999f * currentTimeR_;

    float modTime = getModulatedTime(isLeft);
    int delay = static_cast<int>(modTime * sampleRate_);
    int bufSize = static_cast<int>(bufL_.size());

    auto& buf   = isLeft ? bufL_ : bufR_;
    int&  write = isLeft ? writeL_ : writeR_;

    int readPos = (write + bufSize - delay) % bufSize;
    float delayed = buf[readPos];

    // DC block on feedback path prevents DC accumulation
    float feedback_sig = isLeft ? dcL_.Process(delayed) : dcR_.Process(delayed);

    buf[write] = in + feedback_sig * feedback_;
    write = (write + 1) % bufSize;

    return in * dryMix_ + delayed * wetMix_;
}

void DelayEffect::UpdateUI() {
    if (!controlsActive) {
        controls_.GetLed(0).Set(true);
        controls_.GetLed(1).Set(true);
        return;
    }
    controls_.GetLed(0).Set(effectActive ? 1.f : 0.f);
    controls_.GetLed(1).Set(wetMix_ * 2.f); // brightness shows wet amount
}

} // namespace droubox
