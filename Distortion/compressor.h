// compressor.h
#ifndef COMPRESSOR_H
#define COMPRESSOR_H

#include <cmath>

class Compressor
{
public:
    Compressor();

    void Init(float sampleRateValue);
    void setThreshold(float thresh_dB);
    void setRatio(float ratio);
    void setAttack(float attack_ms);
    void setRelease(float release_ms);
    void setMakeupGain(float gain_dB);
    void setSampleRate(float sampleRate);
    void setBlend(float blendValue);
    float getBlend();
    void setUseRMS(bool useRMS);
    void setKneeWidth(float kneeWidth_dB);
    void UpdateCompressor(float sustainKnob, float attackKnob, float blendKnob);
    float processSample(float inputSample);

private:
    float threshold = -24.f;
    float ratio = 3.f;
    float attackCoeff = 10.f;
    float releaseCoeff = 100.f;
    float makeupGain = 1.f;
    float envelope;
    float sampleRate = 48000.f;

    bool useRMS = false;
    float rmsWindow = 0.01f;

    float kneeWidth = 6.f;
    float blend = 1.0f;

    float calculateEnvelope(float inputLevel);
    float calculateGainReduction(float envelopeLevel);
};

#endif // COMPRESSOR_H