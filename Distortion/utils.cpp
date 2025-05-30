#include "utils.h"
#include "controls.h"
#include <cmath>

// Logarithmic mapping function
float logMap(float input, float sourceValueMin, float sourceValueMax, float minValue, float maxValue)
{
    // hw.PrintLine("low shelf frequency: %d", input);
    float normalizedInput = (input - sourceValueMin) / (sourceValueMax - sourceValueMin);
    normalizedInput = std::fmax(0.0f, std::fmin(normalizedInput, 1.0f)); // Clamp to [0, 1]
    return minValue * std::pow(maxValue / minValue, normalizedInput);
}

float linMap(float input, float sourceValueMin, float sourceValueMax, float minValue, float maxValue)
{
    float normalizedInput = (input - sourceValueMin) / (sourceValueMax - sourceValueMin);
    normalizedInput = std::fmax(0.0f, std::fmin(normalizedInput, 1.0f)); // Clamp entre 0 et 1
    return minValue + normalizedInput * (maxValue - minValue);
}

float powerMap(float input, float sourceValueMin, float sourceValueMax, float minValue, float maxValue, float exponent)
{
    // Normalize input
    float normalizedInput = (input - sourceValueMin) / (sourceValueMax - sourceValueMin);
    normalizedInput = std::fmax(0.0f, std::fmin(normalizedInput, 1.0f)); // Clamp to [0,1]

    // Apply exponent for curve control
    float curvedInput = std::pow(normalizedInput, exponent);

    return minValue + curvedInput * (maxValue - minValue);
}

float dBToLinear(float dB)
{
    return std::pow(10.0f, dB / 20.0f);
}

float linearToDB(float linear)
{
    return 20.0f * std::log10(linear);
}

