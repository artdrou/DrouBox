#include "looper.h"
#include <algorithm>

void CustomLooper::Init(float sampleRate, float bpm, float beatsPerLoop)
{
    this->sampleRate = sampleRate;
    this->bpm = bpm;
    this->beatsPerLoop = beatsPerLoop;

    loopLength = static_cast<size_t>((60.0f / bpm) * beatsPerLoop * sampleRate);
    loopLength = std::min(loopLength, MAX_BUFFER_SIZE);

    buffer.Init();
    buffer.Clear();
    buffer.SetWritePosition(0);
    buffer.SetReadPosition(0);

    isRecording = false;
    isPlaying = false;
}

void CustomLooper::StartRecording(bool activate)
{
    if (activate && !isRecording)
    {
        isRecording = true;
        buffer.SetWritePosition(0);
        buffer.Clear();
    }
}

void CustomLooper::StopRecording(bool activate)
{
    if (activate && isRecording)
    {
        isRecording = false;
        buffer.SetReadPosition(0);
        isPlaying = true;
    }
}

void CustomLooper::StartPlayback(bool activate)
{
    if (activate && !isPlaying)
    {
        isPlaying = true;
        buffer.SetReadPosition(0);
    }
}

void CustomLooper::StopPlayback(bool activate)
{
    if (activate && isPlaying)
    {
        isPlaying = false;
    }
}

void CustomLooper::Reset()
{
    isRecording = false;
    isPlaying = false;
    buffer.Clear();
}

void CustomLooper::SetBpm(float newBpm)
{
    bpm = newBpm;
    loopLength = static_cast<size_t>((60.0f / bpm) * beatsPerLoop * sampleRate);
    loopLength = std::min(loopLength, MAX_BUFFER_SIZE);
    buffer.Clear();
}

void CustomLooper::SetBeatsPerLoop(float beats)
{
    beatsPerLoop = beats;
    loopLength = static_cast<size_t>((60.0f / bpm) * beatsPerLoop * sampleRate);
    loopLength = std::min(loopLength, MAX_BUFFER_SIZE);
    buffer.Clear();
}

float CustomLooper::ProcessSample(float inputSample)
{
    float out = 0.0f;

    if (isRecording && buffer.GetWritePosition() < loopLength)
    {
        buffer.WriteSample(inputSample);
    }

    if (isPlaying)
    {
        out = buffer.ReadSample();
    }

    return out;
}