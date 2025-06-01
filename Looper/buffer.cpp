#include "buffer.h"

// Static SDRAM buffer definition
float DSY_SDRAM_BSS Buffer::sdramBuffer[MAX_BUFFER_SIZE];

void Buffer::Init()
{
    buffer = sdramBuffer;
    bufferLength = MAX_BUFFER_SIZE;
    writePos = 0;
    readPos = 0;
    Clear();
}

void Buffer::Clear()
{
    for (size_t i = 0; i < bufferLength; ++i)
        buffer[i] = 0.0f;
    writePos = 0;
    readPos = 0;
}

void Buffer::WriteSample(float sample)
{
    buffer[writePos] = sample;
    writePos = (writePos + 1) % bufferLength;
}

float Buffer::ReadSample()
{
    float out = buffer[readPos];
    readPos = (readPos + 1) % bufferLength;
    return out;
}

void Buffer::SetReadPosition(size_t pos)
{
    readPos = pos % bufferLength;
}

void Buffer::SetWritePosition(size_t pos)
{
    writePos = pos % bufferLength;
}

size_t Buffer::GetReadPosition() const
{
    return readPos;
}

size_t Buffer::GetWritePosition() const
{
    return writePos;
}

float* Buffer::GetRawPointer()
{
    return buffer;
}