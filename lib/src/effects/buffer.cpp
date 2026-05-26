#include "buffer.h"
#include <cstring>

namespace droubox {

float DSY_SDRAM_BSS Buffer::sdram_[Buffer::MAX_SAMPLES];

void Buffer::Init() {
    writePos_ = 0;
    readPos_  = 0;
    Clear();
}

void Buffer::Clear() {
    memset(sdram_, 0, sizeof(sdram_));
    writePos_ = 0;
    readPos_  = 0;
}

void Buffer::WriteSample(float sample) {
    sdram_[writePos_] = sample;
    writePos_ = (writePos_ + 1) % MAX_SAMPLES;
}

float Buffer::ReadSample() {
    float out = sdram_[readPos_];
    readPos_ = (readPos_ + 1) % MAX_SAMPLES;
    return out;
}

void Buffer::SetReadPosition(size_t pos) {
    readPos_ = pos % MAX_SAMPLES;
}

void Buffer::SetWritePosition(size_t pos) {
    writePos_ = pos % MAX_SAMPLES;
}

size_t Buffer::GetReadPosition() const  { return readPos_; }
size_t Buffer::GetWritePosition() const { return writePos_; }

float* Buffer::GetRawPointer() { return sdram_; }

} // namespace droubox
