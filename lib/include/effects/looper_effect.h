#pragma once
#include "effectsBase.h"
#include "buffer.h"
#include <cstddef>

namespace droubox {

struct LooperConfig {
    size_t knobBpm = 5;   // knob index for BPM
};

class LooperEffect : public EffectBase {
public:
    explicit LooperEffect(Controls& controls, LooperConfig cfg = {})
        : EffectBase(controls), cfg_(cfg) {}

    void Init(float sampleRate);

    void UpdateParameters() override;
    void Process(const float* in, float* out, size_t size) override;
    void UpdateUI() override;

    void StartRecording();
    void StopRecording();
    void StartPlayback();
    void StopPlayback();
    void Reset();

    bool IsRecording() const { return recording_; }
    bool IsPlaying()   const { return playing_; }

private:
    LooperConfig cfg_;
    Buffer  buffer_;
    size_t  recLen_    = 0;
    size_t  writePos_  = 0;
    size_t  readPos_   = 0;
    bool    recording_ = false;
    bool    playing_   = false;
    float   sampleRate_ = 48000.f;
};

} // namespace droubox
