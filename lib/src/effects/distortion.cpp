#include "distortion.h"
#include "mapping.h"
#include "constants.h"
#include "controls.h"
#include <cmath>
#include <algorithm>

namespace droubox {

static float makeupGain(float drive, float k) {
    return std::sqrt(1.f + k) / std::sqrt(1.f + k * drive);
}

void Distortion::Init(float sampleRate) {
    sampleRate_ = sampleRate;
    comp_.Init(sampleRate);
    lowShelf_.init(sampleRate, 200.f, drouAudioLib::ShelfType::LOW_SHELF,  0.f, 1);
    highShelf_.init(sampleRate, 6000.f, drouAudioLib::ShelfType::HIGH_SHELF, 0.f, 1);
}

void Distortion::UpdateParameters() {
    drive_ = controls_.GetKnobs().GetValue(cfg_.knobDrive);
    level_ = controls_.GetKnobs().GetValue(cfg_.knobLevel);

    int dipVal = controls_.GetDips(cfg_.dipGroup).GetValue();
    int clamped = std::clamp(dipVal, 0, static_cast<int>(DistortionType::Count) - 1);
    type_ = static_cast<DistortionType>(clamped);

    comp_.UpdateFromKnobs(
        controls_.GetKnobs().GetValue(cfg_.knobSustain),
        controls_.GetKnobs().GetValue(cfg_.knobAttack),
        controls_.GetKnobs().GetValue(cfg_.knobBlend));
}

void Distortion::Process(const float* in, float* out, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        float s = in[i];
        if (compressorEnabled_) s = comp_.Process(s);
        s = applyDistortion(s, drive_, level_);
        s = lowShelf_.processFilter(s);
        s = highShelf_.processFilter(s);
        out[i] = s;
    }
}

void Distortion::UpdateUI() {
    if (!controlsActive) {
        controls_.GetLed(0).Set(true);
        controls_.GetLed(1).Set(true);
        return;
    }
    controls_.GetLed(0).Set(effectActive);
    controls_.GetLed(1).Set(false);
}

float Distortion::applyDistortion(float in, float drive, float level) const {
    float d = powerMap(drive, 0.f, 1.f, 1.f, 300.f, 1.75f);
    float l = linMap(level, 0.f, 1.f, 0.f, 2.f);

    switch (type_) {
    case DistortionType::HardClip: {
        const float thr = 0.95f;
        float s = std::clamp(in * d, -thr, thr);
        return makeupGain(d, 100.f) * s * l;
    }
    case DistortionType::TanhClip:
        return std::tanh(in * d) * makeupGain(d, 10.f) * l;
    case DistortionType::ArctanClip:
        return std::atan(in * d) * (2.f / static_cast<float>(PI)) * makeupGain(d, 100.f) * l;
    case DistortionType::LogClip: {
        float s   = in * d;
        float sgn = (s > 0.f) - (s < 0.f);
        return makeupGain(d, 1000.f) * l * sgn *
               (std::log10(1.f + std::fabs(s)) / std::log10(1.5f));
    }
    case DistortionType::ExpClip: {
        float s   = in * d;
        float sgn = (s > 0.f) - (s < 0.f);
        return makeupGain(d, 2.5f) * l * sgn * (1.f - std::exp(-std::fabs(s)));
    }
    case DistortionType::SinClip: {
        float dd = powerMap(drive, 0.f, 1.f, 1.f, 200.f, 1.75f);
        return makeupGain(dd, 100.f) * l * std::sin(in * dd);
    }
    case DistortionType::HalfWave: {
        float dd  = powerMap(drive, 0.f, 1.f, 1.f, 200.f, 1.75f);
        float s   = in * dd;
        return makeupGain(dd, 100.f) * l * std::max(s, -0.5f);
    }
    case DistortionType::FullWave: {
        float dd = powerMap(drive, 0.f, 1.f, 1.f, 200.f, 1.75f);
        return makeupGain(dd, 100.f) * l * std::fabs(in * dd);
    }
    case DistortionType::BitCrush: {
        float dd   = powerMap(drive, 0.f, 1.f, 0.f, 1.f, 0.5f);
        int   bits = static_cast<int>(16 - 15 * dd);
        float step = std::pow(2.f, bits);
        return makeupGain(dd, 100.f) * l * std::round(in * dd * step) / step;
    }
    case DistortionType::TubeAmp: {
        float s = in * d;
        float out_s;
        if (s > 0.f) out_s = s / (1.f + 0.5f * s * s);
        else         out_s = 0.8f * s / (1.f + s * s);
        return makeupGain(d, 100.f) * l * out_s;
    }
    default:
        return in;
    }
}

const char* Distortion::TypeName(DistortionType t) {
    switch (t) {
    case DistortionType::HardClip:   return "Hard Clip";
    case DistortionType::TanhClip:   return "Tanh Clip";
    case DistortionType::ArctanClip: return "Arctan Clip";
    case DistortionType::LogClip:    return "Log Clip";
    case DistortionType::ExpClip:    return "Exp Clip";
    case DistortionType::SinClip:    return "Sin Clip";
    case DistortionType::HalfWave:   return "Half Wave";
    case DistortionType::FullWave:   return "Full Wave";
    case DistortionType::BitCrush:   return "Bit Crush";
    case DistortionType::TubeAmp:    return "Tube Amp";
    default:                          return "Unknown";
    }
}

} // namespace droubox
