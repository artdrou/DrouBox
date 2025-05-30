#ifndef DELAY_H
#define DELAY_H

#include <vector>
#include <cmath>
#include "LFO.h"  // Include your LFO class

#define MAX_DELAY_TIME .5f // Maximum delay time in seconds

class Delay
{
public:
    Delay() = default;
    void init(float sampleRate);
    void reset();
    void setParameters(float feedback, float targetDelayTimeLeft, float targetDelayTimeRight, float blend, float modRate, float modDepth);
    float processSample(float inputSample, bool isLeftChannel);

private:
    float sampleRate = 44100.0f;
    float feedback = 0.5f;
    float targetDelayTimeLeft = 0.5f;
    float targetDelayTimeRight = 0.5f;
    float currentDelayTimeLeft = 0.5f;
    float currentDelayTimeRight = 0.5f;
    float dryMix = 1.0f;
    float wetMix = 0.5f;
    float blend = 0.5f;
    float modRate = 2.f;
    float modDepth = 0.1f;
    
    std::vector<float> leftDelayBuffer;
    std::vector<float> rightDelayBuffer;
    int delayBufferSize = 0;
    int leftWritePosition = 0;
    int rightWritePosition = 0;

    LFO lfo; // Use your LFO class

    // Function to modulate the delay time using LFO
    float getModulatedDelayTime(bool isLeftChannel);
    float interpolateDelayTime(float currentTime, float targetTime, float smoothingFactor);
};

#endif // DELAY_H
