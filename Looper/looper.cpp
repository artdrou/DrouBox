#include "looper.h"
#include <algorithm>

void CustomLooper::Init(float sampleRate, BpmRunner& bpmRunnerInstance)
{
    this->sampleRate = sampleRate;
    bpmRunner = &bpmRunnerInstance;

    float bpm_ = bpmRunner->GetBpm();
    int beatsPerLoop_ = bpmRunner->GetBeatsPerMeasure();

    loopLength = static_cast<size_t>((60.0f / bpm_) * beatsPerLoop_ * sampleRate);
    loopLength = std::min(loopLength, MAX_BUFFER_SIZE);

    buffer.Init();
    buffer.Clear();
    buffer.SetWritePosition(0);
    buffer.SetReadPosition(0);

    isRecording = false;
    isPlaying = false;
    isRecordingPending = false;
    useSnap = true; // default if snapping is desired
}

void CustomLooper::Update()
{
    if (isRecordingPending)
    {
        if (bpmRunner->IsMeasure())  // shared instance tells us about measure start
        {
            isRecordingPending = false;
            isRecording = true;
            buffer.SetWritePosition(0);
            // clear buffer if needed
        }
    }

    // other update code...
}

void CustomLooper::UpdateLoopTiming()
{
    bpm_ = bpmRunner->GetBpm();
    beatsPerLoop_ = bpmRunner->GetBeatsPerMeasure();
    loopLength = static_cast<size_t>((60.0f / bpm_) * beatsPerLoop_ * sampleRate);
    loopLength = std::min(loopLength, MAX_BUFFER_SIZE);
    buffer.Clear();
    buffer.SetWritePosition(0);
    buffer.SetReadPosition(0);
}

void CustomLooper::StartRecording(bool activate)
{
    if (activate && !isRecording && !isRecordingPending)
    {
        if (useSnap)  // if snapping enabled
        {
            isRecordingPending = true;
        }
        else
        {
            isRecording = true;
            buffer.Clear();
            buffer.SetWritePosition(0);
        }
    }
}

void CustomLooper::StopRecording(bool activate)
{
    if (activate && (isRecording || isRecordingPending))
    {
        isRecording = false;
        isRecordingPending = false;
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
    isRecordingPending = false;
    buffer.Clear();
    buffer.SetReadPosition(0);
    buffer.SetWritePosition(0);
}

float CustomLooper::ProcessSample(float inputSample)
{
    float outputSample = 0.0f;

    // Record if recording and buffer not full
    if (isRecording)
    {
        if (buffer.GetWritePosition() < loopLength)
            buffer.WriteSample(inputSample);
        else
            isRecording = false;  // stop recording when buffer full automatically
    }

    if (isPlaying)
    {
        outputSample = buffer.ReadSample();
    }

    return outputSample;
}