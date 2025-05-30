#ifndef LOOPER_H
#define LOOPER_H

#include <vector>

class CustomLooper
{
public:
    void init(float* externalBuffer, size_t bufferSize, float sampleRate, float bpm);
    void setLoopLength(float newLengthInSeconds);
    void startRecording(bool activate);
    void stopRecording(bool activate);
    void startPlayback(bool activate);
    void stopPlayback(bool activate);
    void resetLoop();
    void record(float inputSample);
    float play();
    bool isRecording;
    bool isPlaying;

private:
    float* loopBuffer; // Pointer to external SDRAM buffer
    size_t loopBufferSize;
    float sampleRate;
    float bpm;
    int loopLength;
    int maxLoopBufferSize;
    size_t writePosition, readPosition;
    size_t recordedLength = 0;
};

#endif // LOOPER_H
