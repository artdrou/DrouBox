#ifndef BUFFER_H
#define BUFFER_H

#include "daisy_seed.h"

class Buffer
{
  public:
    static constexpr size_t MAX_LOOP_SECONDS = 60;
    static constexpr size_t MAX_BUFFER_SIZE = MAX_LOOP_SECONDS * 48000;

    void Init(float sampleRate, float bpm, float beatsPerLoop);
    void Reset();
    void WriteSample(float sample);
    float ReadSample();
    void AdvanceRead();
    void AdvanceWrite();
    void SetLengthFromBpm(float bpm, float beatsPerLoop);
    void SetLengthSamples(size_t length);
    void SetReadPosition(size_t pos);
    void SetWritePosition(size_t pos);
    size_t GetLoopLength() const;
    void Stop();

  private:
    static float DSY_SDRAM_BSS sdramBuffer[MAX_BUFFER_SIZE];
    float* buffer = sdramBuffer;

    float sampleRate = 48000.f;
    size_t loopLength = 0;
    size_t writePos = 0;
    size_t readPos = 0;
    bool isActive = false;
};


#endif // BUFFER_H