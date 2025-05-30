#include "looper.h"
#include <vector>
#include <algorithm>
#include "daisy_seed.h"
#include "daisy_petal.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;
using namespace daisy::seed;

// Hardware instance
extern DaisySeed hw;

void CustomLooper::init(float* externalBuffer, size_t bufferSize, float sampleRate, float bpm)
{
    this->loopBuffer = externalBuffer;
    this->loopBufferSize = bufferSize;
    this->sampleRate = sampleRate;
    this->bpm = bpm;

    writePosition = 0;
    readPosition = 0;
    isRecording = false;
    isPlaying = false;

    // Clear the buffer to avoid undefined noise
    for(size_t i = 0; i < bufferSize; ++i)
        loopBuffer[i] = 0.0f;
}

void CustomLooper::setLoopLength(float newLengthInSeconds)
{
    int newSize = static_cast<int>(sampleRate * newLengthInSeconds);
    if (newSize <= maxLoopBufferSize) // Ensure we don’t exceed SDRAM
    {
        loopBufferSize = newSize;
    }
}

void CustomLooper::startRecording(bool activate)
{
    if (activate && !isRecording)
    {
        std::fill(loopBuffer, loopBuffer + loopBufferSize, 0.0f); // Reset buffer to silence
        hw.PrintLine("start recording");
        isRecording = true;
        writePosition = 0;
    }
}

void CustomLooper::stopRecording(bool activate)
{
    if (activate && isRecording)
    {
        hw.PrintLine("stop recording");
        isRecording = false;
        recordedLength = writePosition;
        readPosition = 0;
        isPlaying = true;
    }
}

void CustomLooper::resetLoop()
{
    std::fill(loopBuffer, loopBuffer + loopBufferSize, 0.0f); // Reset loop to silence
    writePosition = 0;
    readPosition = 0;
    isRecording = false;
    isPlaying = false;
}

void CustomLooper::startPlayback(bool activate)
{
    if (activate && !isPlaying)
    {
        isPlaying = true;
        readPosition = 0;
    }
}

void CustomLooper::stopPlayback(bool activate)
{
    if (activate && isPlaying)
    {
        isPlaying = false;
    }
}

void CustomLooper::record(float inputSample)
{
    if (isRecording)
    {
        loopBuffer[writePosition++] = inputSample * 0.9f; // Lower recording volume to prevent clipping
        if (writePosition >= loopBufferSize)
            isRecording = false; // Auto-stop if maxed
    }
}

float CustomLooper::play()
{
    float outputSample = 0.f;
    if (isPlaying)
    {
        outputSample = loopBuffer[readPosition] * 0.8f; // Reduce volume to avoid loud playback
        readPosition++;
        if(readPosition >= recordedLength)
            readPosition = 0;
    }
    return outputSample;
}


