#ifndef LOOPER_H
#define LOOPER_H

#include <vector>
#include "buffer.h"
#include "bpm.h"

class CustomLooper
{
    BpmRunner* bpmRunner;

  public:
    void Init(float sampleRate, BpmRunner& bpmRunnerInstance);
    void Update();
    void StartRecording(bool activate);
    void StopRecording(bool activate);
    void StartPlayback(bool activate);
    void StopPlayback(bool activate);
    void Reset();

    void UpdateLoopTiming();

    float ProcessSample(float inputSample);

    bool IsRecording() const { return isRecording; }
    bool IsPlaying() const { return isPlaying; }

  private:
    Buffer buffer;

    float sampleRate = 48000.0f;
    size_t loopLength = 0;

    bool isRecording = false;
    bool isRecordingPending = false;
    bool isPlaying = false;
    bool useSnap = true;

    float bpm_;
    int beatsPerLoop_;
};

#endif // LOOPER_H
