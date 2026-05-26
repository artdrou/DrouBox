#pragma once
#include "daisy_seed.h"
#include <cstddef>

namespace droubox {

class Buffer {
public:
    static constexpr size_t MAX_SAMPLES = 60 * 48000; // 60 s at 48 kHz

    void  Init();
    void  Clear();

    void  WriteSample(float sample);
    float ReadSample();

    void   SetReadPosition(size_t pos);
    void   SetWritePosition(size_t pos);
    size_t GetReadPosition()  const;
    size_t GetWritePosition() const;

    float* GetRawPointer();

private:
    static float sdram_[MAX_SAMPLES]; // DSY_SDRAM_BSS in .cpp
    size_t writePos_ = 0;
    size_t readPos_  = 0;
};

} // namespace droubox
