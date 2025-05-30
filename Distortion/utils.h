// utils.h
#ifndef UTILS_H
#define UTILS_H

// Mapping functions
float logMap(float input, float sourceValueMin, float sourceValueMax, float minValue, float maxValue);
float linMap(float input, float sourceValueMin, float sourceValueMax, float minValue, float maxValue);
float powerMap(float input, float sourceValueMin, float sourceValueMax, float minValue, float maxValue, float exponent);

// Conversion functions
float dBToLinear(float dB);
float linearToDB(float linear);

#endif // UTILS_H