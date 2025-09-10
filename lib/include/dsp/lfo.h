#pragma once
#include <cmath>

namespace drouAudioLib {
class Lfo
{
public:
    enum class Waveform { Sine, Triangle, Sawtooth, Square };

    Lfo();

    void init(float sampleRate, float frequency, Waveform waveform = Waveform::Sine);
    void setFrequency(float frequency);
    void setWaveform(Waveform waveform);
    float getNextSample();

private:
    void updatePhaseIncrement();

    int sampleRate_;
    float phase_;
    float phaseIncrement_;
    float frequency_ = 1.0f;
    Waveform waveform_ = Waveform::Sine;
};
};