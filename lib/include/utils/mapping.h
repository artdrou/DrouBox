#pragma once

// Mapping functions
float logMap(float input, float sourceValueMin, float sourceValueMax, float minValue, float maxValue);
float linMap(float input, float sourceValueMin, float sourceValueMax, float minValue, float maxValue);
float powerMap(float input, float sourceValueMin, float sourceValueMax, float minValue, float maxValue, float exponent);

// Conversion functions
float dBToLinear(float dB);
// Converts a linear amplitude value to decibels (dB). Returns -inf if input is zero or negative.
float linearToDB(float linear);