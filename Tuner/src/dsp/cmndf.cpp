#include "cmndf.h"
#include <cmath>
#include <algorithm>
#include <limits>
#include <utils/mapping.h>

// Step 1: Normalize input buffer
// Remove DC and normalize amplitude
std::vector<float> NormalizeBuffer(const std::vector<float>& in, size_t size) {
    std::vector<float> normalizedBuffer(size);
    // Compute mean
    float mean = 0.0f;
    for (size_t i = 0; i < size; i++) {
        mean += in[i];
    }
    mean /= size;
    // Find max absolute value
    float maxVal = 0.0f;
    for (size_t i = 0; i < size; i++) {
        normalizedBuffer[i] = in[i] - mean;
        if (fabs(normalizedBuffer[i]) > maxVal) {
            maxVal = fabs(normalizedBuffer[i]);
        }
    }
    // Normalize
    if (maxVal > 0.0f) {
        for (size_t i = 0; i < size; i++) {
            normalizedBuffer[i] /= maxVal;
        }
    }
    return normalizedBuffer;
}

// Step 2: Compute difference function
std::vector<float> DifferenceFunction(const std::vector<float>& normalizedBuffer) {
    size_t W = normalizedBuffer.size();
    size_t maxTau = W/2;
    std::vector<float> diff(maxTau + 1, 0.0f);

    for (size_t tau = 0; tau <= maxTau; tau++) {
        float sum = 0.0f;
        for (size_t i = 1; i < W - tau; i++) {
            float d = normalizedBuffer[i] - normalizedBuffer[i + tau];
            sum += d * d;
        }
        diff[tau] = sum;
    }
    return diff;
}

// Step 3: Cumulative mean normalization (CMNDF)
std::vector<float> CumulativeMeanNormalizedDifference(const std::vector<float>& diff) {
    size_t size = diff.size();
    std::vector<float> cmndf(size, 0.0f);

    cmndf[0] = 1.0f; // convention

    float runningSum = 0.0f;
    for (size_t tau = 1; tau < size; tau++) {
        runningSum += diff[tau];
        cmndf[tau] = (runningSum > 0.0f) ? diff[tau] * tau / runningSum : 1.0f;
    }

    return cmndf;
}

// Step 4: Absolute threshold
// int AbsoluteThreshold(const std::vector<float>& cmndf, float threshold = 0.0f) {
//     for (size_t tau = 2; tau < cmndf.size(); tau++) {
//         if (cmndf[tau] <= threshold) {
//             // slide forward to local minimum
//             while (tau + 1 < cmndf.size() && cmndf[tau + 1] < cmndf[tau]) {
//                 tau++;
//             }
//             return static_cast<int>(tau);
//         }
//     }
//     return -1; // no pitch detected
// }
int AbsoluteThreshold(const std::vector<float>& cmndf, float threshold = 0.4f) {
    for (size_t tau = 1; tau + 1 < cmndf.size(); tau++) {
        if (cmndf[tau] < cmndf[tau - 1] && cmndf[tau] < cmndf[tau + 1] && cmndf[tau] < threshold) {
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
float CMNDFPitchDetection(const std::vector<float>& in, size_t size, float sampleRate, Controls& controls) {
    // Step 1: normalize
    auto norm = NormalizeBuffer(in, size);
    // Step 2: difference function
    auto diff = DifferenceFunction(norm); // up to Nyquist
    // Step 3: cumulative mean normalized difference
    auto cmndf = CumulativeMeanNormalizedDifference(diff);
    // // Step 4: find tau
    int tau = AbsoluteThreshold(cmndf, 0.2f);
    // Step 5: convert to frequency
    float frequency = GetPitchFromLag(tau, sampleRate);
    // controls.GetHwPtr()->PrintLine("frequency candidate: %d", int(frequency));
    return frequency
    ;
}

