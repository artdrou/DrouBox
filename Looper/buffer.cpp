#include "buffer.h"

// Static SDRAM buffer definition
float DSY_SDRAM_BSS Buffer::sdramBuffer[MAX_BUFFER_SIZE];

void Buffer::Init(float sampleRate, float bpm, float beatsPerLoop)
{
    this->sampleRate = sampleRate;
    loopLength = static_cast<size_t>((60.0f / bpm) * beatsPerLoop * sampleRate);
    loopLength = std::min(loopLength, MAX_BUFFER_SIZE);

    buffer = sdramBuffer;
    Reset();

    isActive = true;
    writePos = 0;
    readPos = 0;
}

void Buffer::Reset()
{
    for (size_t i = 0; i < loopLength; ++i)
        buffer[i] = 0.0f;

    writePos = 0;
    readPos = 0;
}

void Buffer::WriteSample(float sample)
{
    if (isActive)
    {
        buffer[writePos] = sample;
        AdvanceWrite();
    }
}

float Buffer::ReadSample()
{
    if (!isActive)
        return 0.0f;

    float out = buffer[readPos];
    AdvanceRead();
    return out;
}

void Buffer::AdvanceWrite()
{
    writePos = (writePos + 1) % loopLength;
}

void Buffer::AdvanceRead()
{
    readPos = (readPos + 1) % loopLength;
}

void Buffer::SetLengthFromBpm(float bpm, float beatsPerLoop)
{
    loopLength = static_cast<size_t>((60.0f / bpm) * beatsPerLoop * sampleRate);
    loopLength = std::min(loopLength, MAX_BUFFER_SIZE);
    Reset();
}

void Buffer::SetLengthSamples(size_t length)
{
    loopLength = std::min(length, MAX_BUFFER_SIZE);
    Reset();
}

void Buffer::SetReadPosition(size_t pos)
{
    readPos = pos % loopLength;
}

void Buffer::SetWritePosition(size_t pos)
{
    writePos = pos % loopLength;
}

size_t Buffer::GetLoopLength() const
{
    return loopLength;
}

void Buffer::Stop()
{
    isActive = false;
}
