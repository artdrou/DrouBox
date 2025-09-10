#include "constants.h"
#include "lfo.h"
#include <cmath> // required for std::sin, std::fmod, std::fabs

using namespace drouAudioLib;

Lfo::Lfo() = default;

void Lfo::init(float sampleRate, float frequency, Waveform waveform)
{
    sampleRate_ = sampleRate;
    setFrequency(frequency);
    setWaveform(waveform);
}

void Lfo::setFrequency(float frequency)
{
    frequency_ = frequency;
    updatePhaseIncrement();
}

void Lfo::setWaveform(Waveform waveform)
{
    waveform_ = waveform;
}

void Lfo::updatePhaseIncrement()
{
    phaseIncrement_ = frequency_ / sampleRate_;
}

float Lfo::getNextSample()
{
    float output = 0.0f;

    switch (waveform_)
    {
        case Waveform::Sine:
            output = std::sin(phase_ * TWO_PI);
            break;

        case Waveform::Triangle:
            output = 2.0f * std::abs(2.0f * (phase_ - std::floor(phase_ + 0.5f))) - 1.0f;
            break;

        case Waveform::Sawtooth:
            output = 2.0f * (phase_ - std::floor(phase_ + 0.5f));
            break;

        case Waveform::Square:
            output = (phase_ < 0.5f) ? 1.0f : -1.0f;
            break;
    }

    phase_ += phaseIncrement_;
    if (phase_ >= 1.0f)
        phase_ -= 1.0f;

    return output;
}
