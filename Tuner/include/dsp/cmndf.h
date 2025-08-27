#pragma once
#include <vector>
#include <cstddef>
#include "controls.h"

// Main detection function
float CMNDFPitchDetection(const std::vector<float>& in, float sampleRate, int minFreq = 75);

// Sub-steps (exposed for debugging, but could also be kept private)
std::vector<float> NormalizeBuffer(const std::vector<float>& in);
std::vector<float> Decimate(const std::vector<float>& input, size_t factor);
std::vector<float> DifferenceFunction(const std::vector<float>& buffer, int maxTau);
std::vector<float> CumulativeMeanNormalizedDifference(const std::vector<float>& diff);
float AbsoluteThreshold(const std::vector<float>& cmndf, float threshold);
float ParabolicInterpolation(const std::vector<float>& cmndf, int tau);
float GetPitchFromLag(int tau, float sampleRate);
