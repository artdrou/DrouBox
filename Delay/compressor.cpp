#include "Compressor.h"
#include "utils.h"


// Constructor
Compressor::Compressor() {
    // Initialize member variables, if needed
}

void Compressor::Init(float sampleRateValue)
{
    setSampleRate(sampleRateValue);
}

void Compressor::setThreshold(float thresh_dB)
{
    threshold = dBToLinear(thresh_dB);
}

void Compressor::setRatio(float newRatio)
{
    ratio = newRatio;
}

void Compressor::setAttack(float attack_ms)
{
    attackCoeff = std::exp(-1.0f / ((attack_ms / 1000.0f) * sampleRate));
}

void Compressor::setRelease(float release_ms)
{
    releaseCoeff = std::exp(-1.0f / ((release_ms / 1000.0f) * sampleRate));
}

void Compressor::setMakeupGain(float gain_dB)
{
    makeupGain = dBToLinear(gain_dB);
}

void Compressor::setSampleRate(float sr)
{
    sampleRate = sr;
    setAttack(10.0f);
    setRelease(100.0f);
}

void Compressor::setUseRMS(bool enableRMS)
{
    useRMS = enableRMS;
    rmsWindow = 0.0f;
}

void Compressor::setKneeWidth(float kneeWidth_dB)
{
    kneeWidth = kneeWidth_dB;
}

void Compressor::setBlend(float blendValue)
{
    blend = blendValue;
}

float Compressor::getBlend()
{
    return blend;
}

// Envelope follower
float Compressor::calculateEnvelope(float inputLevel)
{
    float detector = 0.0f;

    if (useRMS)
    {
        rmsWindow = 0.99f * rmsWindow + 0.01f * (inputLevel * inputLevel);
        detector = std::sqrt(rmsWindow + 1e-8f);
    }
    else
    {
        detector = inputLevel;
    }

    if (detector > envelope)
        envelope = attackCoeff * (envelope - detector) + detector;
    else
        envelope = releaseCoeff * (envelope - detector) + detector;

    return envelope;
}

// Handles hard & soft knee gain calculation
float Compressor::calculateGainReduction(float envelopeLevel)
{
    float dBInput = linearToDB(envelopeLevel);
    float dBThreshold = linearToDB(threshold);
    float dBGainReduction = 0.0f;

    if (kneeWidth > 0.0f)
    {
        float halfKnee = kneeWidth / 2.0f;
        float lowerKnee = dBThreshold - halfKnee;
        float upperKnee = dBThreshold + halfKnee;

        if (dBInput <= lowerKnee)
        {
            dBGainReduction = 0.0f;
        }
        else if (dBInput > lowerKnee && dBInput < upperKnee)
        {
            float x = (dBInput - lowerKnee) / (kneeWidth);
            float interpolatedRatio = 1.0f + (ratio - 1.0f) * x * x;
            dBGainReduction = (dBThreshold + (dBInput - dBThreshold) / interpolatedRatio) - dBInput;
        }
        else
        {
            dBGainReduction = (dBThreshold + (dBInput - dBThreshold) / ratio) - dBInput;
        }
    }
    else
    {
        if (envelopeLevel > threshold)
        {
            dBGainReduction = (dBThreshold + (dBInput - dBThreshold) / ratio) - dBInput;
        }
    }

    return dBToLinear(dBGainReduction);
}
void Compressor::UpdateCompressor(float sustainKnob, float attackKnob, float blendKnob)
{
    float threshold = linMap(sustainKnob, 0.f, 1.f, 0.f, -40.f);
    float ratio = linMap(sustainKnob, 0.f, 1.f, 1.f, 8.f);
    float attack = linMap(attackKnob, 0.f, 1.f, 5.f, 200.f);
    float blend = linMap(blendKnob, 0.f, 1.f, 0.f, 1.f);

    setThreshold(threshold);
    setRatio(ratio);
    setAttack(attack);
    setBlend(blend);
}

float Compressor::processSample(float inputSample) {
    // Calculate the absolute value of the input sample
    float inputLevel = std::fabs(inputSample);
    // Avoid a divide-by-zero error
    if (inputLevel < 1e-6f)
        inputLevel = 1e-6f;
    // Get the envelope (attack/release dynamics applied)
    float currentEnvelope = calculateEnvelope(inputLevel);
    // Calculate the gain reduction based on the envelope level
    float gain = calculateGainReduction(currentEnvelope);
    // Apply the compression to the input sample (wet signal)
    float processedSample = inputSample * gain * makeupGain;
    // Blend between dry and wet signals using the blend parameter
    float outputSample = (1.0f - blend) * inputSample + blend * processedSample;
    return outputSample;
}
