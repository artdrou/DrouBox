#include "cmndf.h"
#include <cmath>
#include <algorithm>
#include <limits>
#include <utils/mapping.h>

// Step 1: Normalize input buffer
// Remove DC and normalize amplitude
std::vector<float> NormalizeBuffer(const float* in, size_t size) {
    std::vector<float> buffer(size);
    // Compute mean
    float mean = 0.0f;
    for (size_t i = 0; i < size; i++) {
        mean += in[i];
    }
    mean /= size;
    // Find max absolute value
    float maxVal = 0.0f;
    for (size_t i = 0; i < size; i++) {
        buffer[i] = in[i] - mean;
        if (fabs(buffer[i]) > maxVal) {
            maxVal = fabs(buffer[i]);
        }
    }
    // Normalize
    if (maxVal > 0.0f) {
        for (size_t i = 0; i < size; i++) {
            buffer[i] /= maxVal;
        }
    }
    return buffer;
}

// Step 2: Compute difference function
std::vector<float> DifferenceFunction(const std::vector<float>& buffer) {
    size_t size = buffer.size();
    std::vector<float> diff(size, 0.0f);

    for (size_t tau = 1; tau < size; tau++) {
        double sum = 0.0f;
        for (size_t i = 0; i < size - tau; i++) {
            float d = buffer[i] - buffer[i + tau];
            sum += d * d;
        }
        diff[tau] = static_cast<float>(sum);
    }

    return diff;
}

// Step 3: Cumulative mean normalization (CMNDF)
std::vector<float> CumulativeMeanNormalizedDifference(const std::vector<float>& diff) {
    size_t size = diff.size();
    std::vector<float> cmndf(size, 0.0f);

    float runningSum = 0.0f;
    cmndf[0] = 1.0f; // convention
    for (size_t tau = 1; tau < size; tau++) {
        runningSum += diff[tau];
        cmndf[tau] = (runningSum > 0.0f) ? diff[tau] * tau / runningSum : 1.0f;
    }
    // --- scaling using linMap ---
    float minVal = cmndf[1];
    float maxVal = cmndf[1];
    for (size_t i = 1; i < size; i++) {
        if (cmndf[i] < minVal) minVal = cmndf[i];
        if (cmndf[i] > maxVal) maxVal = cmndf[i];
    }

    for (size_t i = 1; i < size; i++) {
        cmndf[i] = linMap(cmndf[i], minVal, maxVal, 0.0f, 1.0f);
    }
    return cmndf;
}

// Step 4: Absolute threshold
int AbsoluteThreshold(const std::vector<float>& cmndf, float threshold = 0.0f) {
    for (size_t tau = 2; tau < cmndf.size(); tau++) {
        if (cmndf[tau] <= threshold) {
            // slide forward to local minimum
            while (tau + 1 < cmndf.size() && cmndf[tau + 1] < cmndf[tau]) {
                tau++;
            }
            return static_cast<int>(tau);
        }
    }
    return -1; // no pitch detected
}

// Step 5: Convert lag to frequency
float GetPitchFromLag(int tau, float sampleRate) {
    if (tau <= 0) return -1.0f;
    return sampleRate / static_cast<float>(tau);
}

// Main function
float CMNDFPitchDetection(const float* in, size_t size, float sampleRate, Controls& controls) {
    // Step 1: normalize
    auto norm = NormalizeBuffer(in, size);
    // Step 2: difference function
    auto diff = DifferenceFunction(norm); // up to Nyquist
    controls.GetHwPtr()->PrintLine("diff[100] x1000: %d, diff[200] x1000: %d", (int)(diff[100]), (int)(diff[200]));
    // // Step 3: cumulative mean normalized difference
    // auto cmndf = CumulativeMeanNormalizedDifference(diff);
    // // int minVal = 1000000; // large initial value
    // // for (size_t i = 1; i < cmndf.size(); i++) { // skip tau=0
    // //     int v = (int)(cmndf[i] * 1000.f); // scale to int
    // //     if (v < minVal) minVal = v;
    // // }
    // // controls.GetHwPtr()->PrintLine("min cmndf: %d", minVal);
    // // Step 4: find tau
    // int tau = AbsoluteThreshold(cmndf);
    // // controls.GetHwPtr()->PrintLine("tau candidate: %d", tau);
    // // Step 5: convert to frequency
    // float frequency = GetPitchFromLag(tau, sampleRate);
    return 1;
}

