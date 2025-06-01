#ifndef BUFFER_H
#define BUFFER_H

#include "daisy_seed.h"

static constexpr size_t MAX_BUFFER_SIZE = 60 * 48000; // 60 seconds max

class Buffer
{
  public:
    void Init();
    void Clear();

    void WriteSample(float sample);
    float ReadSample();

    void SetReadPosition(size_t pos);
    void SetWritePosition(size_t pos);

    size_t GetReadPosition() const;
    size_t GetWritePosition() const;

    float* GetRawPointer();

  private:
    static float sdramBuffer[MAX_BUFFER_SIZE];
    float* buffer = nullptr;
    size_t bufferLength = MAX_BUFFER_SIZE;

    size_t writePos = 0;
    size_t readPos = 0;
};


#endif // BUFFER_H