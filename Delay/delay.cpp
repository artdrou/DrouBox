#include "delay.h"
#include <utils/mapping.h>

// Prepare delay buffers and parameters
void Delay::init(float sampleRate)
{
    this->sampleRate = sampleRate;
    delayBufferSize = static_cast<int>(sampleRate * MAX_DELAY_TIME);
    
    leftDelayBuffer.assign(delayBufferSize, 0.0f);
    rightDelayBuffer.assign(delayBufferSize, 0.0f);
    
    leftWritePosition = 0;
    rightWritePosition = 0;

    // Initialize the LFO with sampleRate and modulation rate
    lfo.init(sampleRate, modRate, drouAudioLib::Lfo::Waveform::Sine);
}

// Reset delay buffers
void Delay::reset()
{
    std::fill(leftDelayBuffer.begin(), leftDelayBuffer.end(), 0.0f);
    std::fill(rightDelayBuffer.begin(), rightDelayBuffer.end(), 0.0f);
    leftWritePosition = 0;
    rightWritePosition = 0;
}

// Set parameters for delay
void Delay::setParameters(float feedback, float targetDelayTimeLeft, float targetDelayTimeRight, float blend, float modRate, float modDepth)
{
    this->feedback = linMap(feedback, 0.f, 1.f, 0.0f, 0.9f);
    this->targetDelayTimeLeft = linMap(targetDelayTimeLeft, 0.f, 1.f, 0.001f, 0.25f);
    this->targetDelayTimeRight = linMap(targetDelayTimeRight, 0.f, 1.f, 0.001f, 0.25f);
    this->blend = blend;
    this->dryMix = linMap(blend, 0.f, 1.f, 0.5f, 0.f);
    this->wetMix = linMap(blend, 0.f, 1.f, 0.f, 0.5f);;
    this->modRate = linMap(modRate, 0.f, 1.f, 0.1f, 3.f);
    this->modDepth = linMap(modDepth, 0.f, 1.f, 0.f, 0.01f);

    // Update LFO rate
    lfo.setFrequency(modRate);
}

// Function to modulate the delay time using the LFO
float Delay::getModulatedDelayTime(bool isLeftChannel)
{
    // Get the LFO value and modulate delay time
    float lfoValue = lfo.getNextSample();
    // Apply modulation depth, separately for left and right channels
    if (isLeftChannel)
        return currentDelayTimeLeft + (modDepth * lfoValue);  // Apply modulation to left channel
    else
        return currentDelayTimeRight + (modDepth * lfoValue); // Apply modulation to right channel
}

float Delay::interpolateDelayTime(float currentTime, float targetTime, float smoothingFactor)
{
    return (smoothingFactor * targetTime) + ((1.0f - smoothingFactor) * currentTime);
}

// Process sample with stereo delay and modulation
float Delay::processSample(float inputSample, bool isLeftChannel)
{
    currentDelayTimeLeft = interpolateDelayTime(currentDelayTimeLeft, targetDelayTimeLeft, 0.001f);
    currentDelayTimeRight = interpolateDelayTime(currentDelayTimeRight, targetDelayTimeRight, 0.001f);
    // Modulate delay time using LFO
    float modulatedDelayTime = getModulatedDelayTime(isLeftChannel);
    
    int delaySamples = static_cast<int>(modulatedDelayTime * sampleRate);
    
    int delayReadPosition = 0;
    if (isLeftChannel)
    {
        delayReadPosition = (leftWritePosition + delayBufferSize - delaySamples) % delayBufferSize;
    }
    else
    {
        delayReadPosition = (rightWritePosition + delayBufferSize - delaySamples) % delayBufferSize;
    }

    // Get delayed sample from respective buffer
    float delayedSample = isLeftChannel ? leftDelayBuffer[delayReadPosition] : rightDelayBuffer[delayReadPosition];

    // Apply feedback to the input and write to the buffer
    if (isLeftChannel)
    {
        leftDelayBuffer[leftWritePosition] = inputSample + (delayedSample * feedback);
        leftWritePosition = (leftWritePosition + 1) % delayBufferSize;
    }
    else
    {
        rightDelayBuffer[rightWritePosition] = inputSample + (delayedSample * feedback);
        rightWritePosition = (rightWritePosition + 1) % delayBufferSize;
    }

    // Blend dry and wet signals and return the processed sample
    return inputSample * dryMix + delayedSample * wetMix;
}
