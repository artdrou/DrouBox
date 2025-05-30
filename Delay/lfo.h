// lfo.h
#ifndef LFO_H
#define LFO_H

#define PI 3.1415926535897932384626433f

class LFO
{
public:
    enum class Waveform { Sine, Triangle, Sawtooth, Square };

    LFO() = default;
    void initialize(float sampleRate, float frequency, Waveform waveform = Waveform::Sine);
    void setFrequency(float frequency);
    void setWaveform(Waveform waveform);
    float getNextSample();

private:
    float sampleRate = 44100.0f;
    float frequency = 10.0f;
    Waveform waveform = Waveform::Sine;
    float phase = 0.0f;
    float phaseIncrement = 0.0f;
};

#endif // LFO_H