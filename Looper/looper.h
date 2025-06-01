#ifndef LOOPER_H
#define LOOPER_H

#include <vector>
#include "buffer.h"

class CustomLooper
{
  public:
    void Init(float sampleRate, float bpm, float beatsPerLoop);

    void StartRecording(bool activate);
    void StopRecording(bool activate);
    void StartPlayback(bool activate);
    void StopPlayback(bool activate);
    void Reset();

    void SetBpm(float newBpm);
    void SetBeatsPerLoop(float beats);

    float ProcessSample(float inputSample);

    bool IsRecording() const { return isRecording; }
    bool IsPlaying() const { return isPlaying; }

  private:
    Buffer buffer;

    float sampleRate = 48000.0f;
    float bpm = 120.0f;
    float beatsPerLoop = 4.0f;
    size_t loopLength = 0;

    bool isRecording = false;
    bool isPlaying = false;
};

#endif // LOOPER_H
