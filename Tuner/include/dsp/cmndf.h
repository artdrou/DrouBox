#pragma once
#include <vector>
#include <cstddef>
#include "controls.h"

// Main detection function
float CMNDFPitchDetection(const float* in, size_t size, float sampleRate, Controls& controls);

// Sub-steps (exposed for debugging, but could also be kept private)
std::vector<float> NormalizeBuffer(const float* in, size_t size);
std::vector<float> DifferenceFunction(const std::vector<float>& buffer);
std::vector<float> CumulativeMeanNormalizedDifference(const std::vector<float>& diff);
int AbsoluteThreshold(const std::vector<float>& cmndf, float threshold);
float GetPitchFromLag(int tau, float sampleRate);
