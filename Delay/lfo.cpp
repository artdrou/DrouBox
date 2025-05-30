#include "LFO.h"

void LFO::initialize(float sampleRate, float frequency, Waveform waveform)
{
    this->sampleRate = sampleRate;
    setFrequency(frequency);
    setWaveform(waveform);
}

void LFO::setFrequency(float frequency)
{
    this->frequency = frequency;
    phaseIncrement = (2.0f * PI * frequency) / sampleRate;
}

void LFO::setWaveform(Waveform waveform)
{
    this->waveform = waveform;
}

float LFO::getNextSample()
{
    float sample = 0.0f;
    
    switch (waveform)
    {
        case Waveform::Sine:
            sample = std::sin(phase);
            break;
        case Waveform::Triangle:
            sample = 2.0f * std::fabs(2.0f * (phase / (2.0f * PI)) - 1.0f) - 1.0f;
            break;
        case Waveform::Sawtooth:
            sample = 2.0f * (phase / (2.0f * PI)) - 1.0f;
            break;
        case Waveform::Square:
            sample = (phase < PI) ? 1.0f : -1.0f;
            break;
    }
    
    phase = std::fmod(phase + phaseIncrement, 2.0f * PI);
    
    return sample;
}