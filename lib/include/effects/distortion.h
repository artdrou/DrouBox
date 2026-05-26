#pragma once
#include "effectsBase.h"
#include "compressor.h"
#include "shelf.h"

namespace droubox {

enum class DistortionType {
    HardClip = 0,
    TanhClip,
    ArctanClip,
    LogClip,
    ExpClip,
    SinClip,
    HalfWave,
    FullWave,
    BitCrush,
    TubeAmp,
    Count
};

struct DistortionConfig {
    size_t knobDrive  = 0;
    size_t knobLevel  = 2;
    size_t knobSustain = 3;
    size_t knobAttack = 4;
    size_t knobBlend  = 5;
    size_t dipGroup   = 0; // which DipManager index selects distortion type
};

class Distortion : public EffectBase {
public:
    explicit Distortion(Controls& controls, DistortionConfig cfg = {})
        : EffectBase(controls), cfg_(cfg) {}

    void Init(float sampleRate);
    void UpdateParameters() override;
    void Process(const float* in, float* out, size_t size) override;
    void UpdateUI() override;

    void SetType(DistortionType t) { type_ = t; }
    void SetDrive(float drive)     { drive_ = drive; }
    void SetLevel(float level)     { level_ = level; }

    static const char* TypeName(DistortionType t);

private:
    DistortionConfig cfg_;
    DistortionType   type_  = DistortionType::TanhClip;
    float drive_ = 0.5f;
    float level_ = 0.5f;
    float sampleRate_ = 48000.f;

    Compressor comp_;
    drouAudioLib::ShelfFilter lowShelf_;
    drouAudioLib::ShelfFilter highShelf_;
    bool compressorEnabled_ = true;

    float applyDistortion(float in, float drive, float level) const;
    static float computeMakeupGain(float drive, float k);
};

} // namespace droubox
