#include "cmndf.h"
#include <cmath>
#include <algorithm>
#include <limits>
#include <utils/mapping.h>

// Step 1: Normalize input buffer
// Remove DC and normalize amplitude
std::vector<float> NormalizeBuffer(const std::vector<float>& in) {
    size_t size = in.size();
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

// Simple downsampling: keep every Nth sample
std::vector<float> Decimate(const std::vector<float>& input, size_t factor) {
    std::vector<float> output;
    output.reserve(input.size() / factor + 1);

    for (size_t i = 0; i < input.size(); i += factor) {
        output.push_back(input[i]);
    }

    return output;
}

// Step 2: Compute difference function
std::vector<float> DifferenceFunction(const std::vector<float>& normalizedBuffer, int maxTau) {
    size_t W = normalizedBuffer.size();
    std::vector<float> diff(maxTau + 1, 0.0f);
    for (int tau = 0; tau <= maxTau; tau++) {
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
float AbsoluteThreshold(const std::vector<float>& cmndf, float threshold = 0.2f) {
    for (size_t tau = 1; tau + 1 < cmndf.size(); tau++) {
        if (cmndf[tau] < cmndf[tau - 1] && cmndf[tau] < cmndf[tau + 1] && cmndf[tau] < threshold) {
            return static_cast<float>(tau);
        }
    }
    return -1.f; // no pitch detected
}

// Quadratic interpolation around the minimum
float ParabolicInterpolation(const std::vector<float>& cmndf, float tau) {
    if (tau <= 0 || tau >= (int)cmndf.size() - 1) {
        return (float)tau; // can't interpolate at boundaries
    }

    float y1 = cmndf[tau - 1];
    float y2 = cmndf[tau];
    float y3 = cmndf[tau + 1];

    // Formula for vertex of parabola
    float denom = (y1 - 2 * y2 + y3);
    if (fabs(denom) < 1e-12) {
        return (float)tau; // avoid division by zero
    }

    float offset = 0.5f * (y1 - y3) / denom;
    return (float)tau + offset;
}


// Step 5: Convert lag to frequency
float GetPitchFromLag(int tau, float sampleRate) {
    if (tau <= 0.f) return -1.0f;
    return sampleRate / static_cast<float>(tau);
}

// Main function
float CMNDFPitchDetection(const std::vector<float>& in, float sampleRate, int minFreq) {
    // Step 1: decimate
    size_t decimationFactor = 8; // tune as needed
    float correctionFactor = 1.f; // measured with 440Hz
    // float correctionFactor = .991f; // measured with 440Hz
    float decimatedFs = correctionFactor * sampleRate / static_cast<float>(decimationFactor);
    auto decim = Decimate(in, decimationFactor);
    int maxTau = std::min(static_cast<int>(decim.size()/2 - 1), static_cast<int>(decimatedFs / minFreq));
    // Step 2: normalize
    auto norm = NormalizeBuffer(decim);
    // Step 3: difference function
    auto diff = DifferenceFunction(norm, maxTau);
    // Step 4: cumulative mean normalized difference
    auto cmndf = CumulativeMeanNormalizedDifference(diff);
    // Step 5: find tau
    float tau = AbsoluteThreshold(cmndf, 0.2f);
    // Step 6: Parabolic interpolation
    if (tau > 0) {
        tau = ParabolicInterpolation(cmndf, tau);
    }
    // Step 7: convert to frequency
    float frequency = GetPitchFromLag(tau, decimatedFs);
    return frequency
    ;
}

