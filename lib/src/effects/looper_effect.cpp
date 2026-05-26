#include "looper_effect.h"
#include "controls.h"
#include <algorithm>

namespace droubox {

void LooperEffect::Init(float* externalBuffer, size_t bufferSamples, float sampleRate) {
    buf_        = externalBuffer;
    bufSize_    = bufferSamples;
    sampleRate_ = sampleRate;
    Reset();
}

void LooperEffect::Reset() {
    if (buf_) std::fill(buf_, buf_ + bufSize_, 0.f);
    writePos_ = readPos_ = recLen_ = 0;
    recording_ = playing_ = false;
}

void LooperEffect::StartRecording() {
    if (recording_) return;
    std::fill(buf_, buf_ + bufSize_, 0.f);
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

void LooperEffect::UpdateParameters() {
    // BPM and other parameter reads happen in the firmware main loop
    // to keep footswitch logic outside the audio-rate callback.
}

void LooperEffect::Process(const float* in, float* out, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        float input = in[i];
        float looped = 0.f;

        if (recording_ && buf_) {
            buf_[writePos_++] = input;
            if (writePos_ >= bufSize_) {
                recording_ = false;
                recLen_    = bufSize_;
                readPos_   = 0;
                playing_   = true;
            }
        }

        if (playing_ && buf_ && recLen_ > 0) {
            looped = buf_[readPos_++];
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
