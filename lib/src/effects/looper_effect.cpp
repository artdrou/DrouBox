#include "looper_effect.h"
#include "controls.h"

namespace droubox {

void LooperEffect::Init(float sampleRate) {
    sampleRate_ = sampleRate;
    buffer_.Init();
    Reset();
}

void LooperEffect::Reset() {
    buffer_.Clear();
    writePos_ = readPos_ = recLen_ = 0;
    recording_ = playing_ = false;
}

void LooperEffect::StartRecording() {
    if (recording_) return;
    buffer_.Clear();
    writePos_ = 0;
    recording_ = true;
    playing_   = false;
}

void LooperEffect::StopRecording() {
    if (!recording_) return;
    recording_ = false;
    recLen_    = writePos_;
    readPos_   = 0;
    playing_   = (recLen_ > 0);
}

void LooperEffect::StartPlayback() {
    if (recLen_ == 0) return;
    playing_  = true;
    readPos_  = 0;
}

void LooperEffect::StopPlayback() {
    playing_ = false;
}

void LooperEffect::UpdateParameters() {}

void LooperEffect::Process(const float* in, float* out, size_t size) {
    float* buf = buffer_.GetRawPointer();

    for (size_t i = 0; i < size; ++i) {
        float input  = in[i];
        float looped = 0.f;

        if (recording_) {
            buf[writePos_++] = input;
            if (writePos_ >= Buffer::MAX_SAMPLES) {
                recording_ = false;
                recLen_    = Buffer::MAX_SAMPLES;
                readPos_   = 0;
                playing_   = true;
            }
        }

        if (playing_ && recLen_ > 0) {
            looped = buf[readPos_++];
            if (readPos_ >= recLen_) readPos_ = 0;
        }

        out[i] = input + looped;
    }
}

void LooperEffect::UpdateUI() {
    if (!controlsActive) {
        controls_.GetLed(0).Set(true);
        controls_.GetLed(1).Set(true);
        return;
    }
    controls_.GetLed(0).Set(recording_);
    controls_.GetLed(1).Set(playing_ && !recording_);
}

} // namespace droubox
