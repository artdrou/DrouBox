#include "disto.h"
#include "utils.h"
#include <cmath>
#include <algorithm> // For std::clamp
#define PI 3.1415926535897932384626433f

// Initializations
int currentTypeInt = 0;
DistortionType currentTypeEnum = intToDistortionType(currentTypeInt);
DistortionType lastTypeEnum = NUM_DISTORTIONS; // Invalid initial

// Converts int input safely to DistortionType enum
DistortionType intToDistortionType(int input)
{
    int clamped = input;
    if (clamped < 0)
        clamped = 0;
    else if (clamped >= NUM_DISTORTIONS)
        clamped = NUM_DISTORTIONS - 1;
    return static_cast<DistortionType>(clamped);
}

// Distortion names array
const char* distortionNames[NUM_DISTORTIONS] = {
    "Hard Clip",
    "Tanh Clip",
    "Arctan Clip",
    "Log Clip",
    "Exp Clip",
    "Sin Clip",
    "Half Wave Clip",
    "Full Wave Reflection",
    "Bit Crush",
    "Tube Amp"
};

float computeMakeupGain(float drive, float k) {
    return sqrt(1.0f + k) / sqrt(1.0f + k * drive);
}

float hardClip(float inputSample, float drive, float level)
{
    drive = powerMap(drive, 0.f, 1.f, 1.f, 300.f, 1.75f);
    level = linMap(level, 0.f, 1.f, 0.f, 2.f);
    const float threshold = 0.95f;
    float amplified = inputSample*drive;
    if (amplified > threshold) {
        amplified = threshold; // Clipping haut
    }
    else if (amplified < -threshold) {
        amplified = -threshold; // Clipping bas
    }
    amplified = computeMakeupGain(drive, 100.f)*amplified*level;
    return amplified;
}

float tanhClip(float inputSample, float drive, float level)
{
    drive = powerMap(drive, 0.f, 1.f, 1.f, 300.f, 1.75f);
    level = linMap(level, 0.f, 1.f, 0.f, 2.f);
    return tanh(inputSample*drive)*computeMakeupGain(drive, 10.f)*level;
}

float arctanClip(float inputSample, float drive, float level)
{
    drive = powerMap(drive, 0.f, 1.f, 1.f, 300.f, 1.75f);
    level = linMap(level, 0.f, 1.f, 0.f, 2.f);
    return atan(inputSample * drive)*(2.f / PI)*computeMakeupGain(drive, 100.f)*level;
}

float logClip(float inputSample, float drive, float level)
{   
    drive = powerMap(drive, 0.f, 1.f, 1.f, 300.f, 1.75f);
    level = linMap(level, 0.f, 1.f, 0.f, 2.f);
    double input = drive * inputSample;
    double sign = (input > 0) - (input < 0);
    double clippedValue = sign * (std::log10(1+std::abs(input))/ std::log10(1+0.5f));
    return computeMakeupGain(drive, 1000.f)*level*clippedValue;
}

float expClip(float inputSample, float drive, float level)
{
    drive = powerMap(drive, 0.f, 1.f, 1.f, 300.f, 1.75f);
    level = linMap(level, 0.f, 1.f, 0.f, 2.f);
    double input = drive * inputSample;
    double sign = (input > 0) - (input < 0);
    double clippedValue = sign * (1.0 - std::exp(-std::abs(input)));
    return computeMakeupGain(drive, 2.5f)*level * clippedValue;
}

float sinClip(float inputSample, float drive, float level)
{
    drive = powerMap(drive, 0.f, 1.f, 1.f, 200.f, 1.75f);
    level = linMap(level, 0.f, 1.f, 0.f, 2.f);
    double input = drive * inputSample;
    double clippedValue = sin(input);
    return computeMakeupGain(drive, 100.f)*level * clippedValue;
}

float halfWaveClip(float inputSample, float drive, float level)
{
    drive = powerMap(drive, 0.f, 1.f, 1.f, 200.f, 1.75f);
    level = linMap(level, 0.f, 1.f, 0.f, 2.f);
    double input = drive * inputSample;
    const float threshold = -0.5f;
    double clippedValue = input;
    if (input < threshold) {
        clippedValue = threshold; // Clipping bas
    }
    return computeMakeupGain(drive, 100.f)*level * clippedValue;
}

float fullWaveReflection(float inputSample, float drive, float level)
{
    drive = powerMap(drive, 0.f, 1.f, 1.f, 200.f, 1.75f);
    level = linMap(level, 0.f, 1.f, 0.f, 2.f);
    double input = drive * inputSample;
    double clippedValue = std::abs(input);
    return computeMakeupGain(drive, 100.f)*level * clippedValue;
}

float bitCrush(float inputSample, float drive, float level)
{
    drive = powerMap(drive, 0.f, 1.f, 0.f, 1.f, 0.5f);
    level = linMap(level, 0.f, 1.f, 0.f, 2.f);
    int nBits = int(16-(15*(drive)));
    double input = drive*inputSample;
    double clippedValue = std::round(input*pow(2, nBits))/pow(2, nBits);
    return computeMakeupGain(drive, 100.f)*level * clippedValue;
}

float tubeAmp(float inputSample, float drive, float level)
{
    drive = powerMap(drive, 0.f, 1.f, 1.f, 200.f, 1.75f);
    level = linMap(level, 0.f, 1.f, 0.f, 2.f);
    float a = 0.5f;
    float b = 0.8f;
    float c = 1.f;
    double clippedValue = 0.f;
    if (drive * inputSample > 0) {
        clippedValue = drive * inputSample / (1 + (a * pow(drive * inputSample, 2)));
    }
    else {
        clippedValue = (b * drive * inputSample) / (1 + (c * pow(drive * inputSample, 2)));
    }
    return computeMakeupGain(drive, 100.f)*level*clippedValue;
}

float applyDistortion(int type, float inputSample, float drive, float level)
{
    switch (type)
    {
    case 0:
        return hardClip(inputSample, drive, level);
    case 1:
        return tanhClip(inputSample, drive, level);
    case 2:
        return arctanClip(inputSample, drive, level);
    case 3:
        return logClip(inputSample, drive, level);
    case 4:
        return expClip(inputSample, drive, level);
    case 5:
        return sinClip(inputSample, drive, level);
    case 6:
        return halfWaveClip(inputSample, drive, level);
    case 7:
        return fullWaveReflection(inputSample, drive, level);
    case 8:
        return bitCrush(inputSample, drive, level);
    case 9:
        return tubeAmp(inputSample, drive, level);
    default:
        // Default case if invalid type
        return inputSample;
    }
}

float processDistortionFromDipsValue(int typeInt, float inputSample, float drive, float level)
{
    DistortionType typeEnum = intToDistortionType(typeInt);
    return applyDistortion(typeEnum, inputSample, drive, level);
}