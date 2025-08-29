#include "yin.h"
#include <cmath>
#include <algorithm>
#include <limits>

std::vector<float> Yin::NormalizeBuffer(const std::vector<float>& in) {
    size_t size = in.size();
    std::vector<float> normalizedBuffer(size);
    float mean = 0.0f;
    for (size_t i = 0; i < size; i++) {
        mean += in[i];
    }
    mean /= size;
    float maxVal = 0.0f;
    for (size_t i = 0; i < size; i++) {
        normalizedBuffer[i] = in[i] - mean;
        maxVal = std::max(maxVal, std::fabs(normalizedBuffer[i]));
    }
    if (maxVal > 0.0f) {
        for (size_t i = 0; i < size; i++) {
            normalizedBuffer[i] /= maxVal;
        }
    }
    return normalizedBuffer;
}

std::vector<float> Yin::Decimate(const std::vector<float>& input) {
    std::vector<float> output;
    output.reserve(input.size() / decimationFactor_ + 1);
    for (size_t i = 0; i < input.size(); i += decimationFactor_) {
        output.push_back(input[i]);
    }
    return output;
}

std::vector<float> Yin::DifferenceFunction(const std::vector<float>& normalizedBuffer) {
    size_t W = normalizedBuffer.size();
    int safeMaxTau = std::min(maxTau_, static_cast<int>(W - 1));

    std::vector<float> diff(safeMaxTau + 1, 0.0f);

    for (int tau = 0; tau <= safeMaxTau; tau++) {
        float sum = 0.0f;
        for (size_t i = 0; i + tau < W; i++) {
            float d = normalizedBuffer[i] - normalizedBuffer[i + tau];
            sum += d * d;
        }
        diff[tau] = sum;
    }
    return diff;
}

std::vector<float> Yin::CumulativeMeanNormalizedDifference(const std::vector<float>& diff) {
    int safeMaxTau = std::min(maxTau_, static_cast<int>(diff.size() - 1));
    std::vector<float> cmndf(safeMaxTau - minTau_, 0.0f);

    for (int tau = minTau_; tau < safeMaxTau; tau++) {
        float runningSum = 0.0f;
        for (int i = 1; i <= tau; i++) {
            runningSum += diff[i];
        }
        if (tau == 0) {
            cmndf[tau - minTau_] = 1.0f;
        } else {
            cmndf[tau - minTau_] = (runningSum > 0.0f) ? diff[tau] * tau / runningSum : 1.0f;
        }
    }
    return cmndf;
}

float Yin::AbsoluteThreshold(const std::vector<float>& cmndf) {
    int safeMaxTau = static_cast<int>(cmndf.size()) - 2;
    for (int tau = 1; tau <= safeMaxTau; tau++) {
        if (cmndf[tau] < cmndf[tau - 1] && cmndf[tau] < cmndf[tau + 1] && cmndf[tau] < threshold_) {
            return static_cast<float>(tau);
        }
    }
    return -1.f;
}

float Yin::ParabolicInterpolation(const std::vector<float>& cmndf, float tau) {
    if (tau <= 0 || tau >= (int)cmndf.size() - 1) {
        return tau;
    }

    float y1 = cmndf[tau - 1];
    float y2 = cmndf[tau];
    float y3 = cmndf[tau + 1];

    float denom = (y1 - 2 * y2 + y3);
    if (std::fabs(denom) < 1e-12) {
        return tau;
    }

    float offset = 0.5f * (y1 - y3) / denom;
    return tau + offset;
}

float Yin::GetPitchFromLag(int tau) {
    if (tau <= 0.f) return -1.0f;
    return decimatedSampleRate_ / static_cast<float>(tau);
}

float Yin::DetectPitch(const std::vector<float>& in) {
    auto decim = Decimate(in);
    auto norm = NormalizeBuffer(decim);
    auto diff = DifferenceFunction(norm);
    auto cmndf = CumulativeMeanNormalizedDifference(diff);
    float tau = AbsoluteThreshold(cmndf);
    if (tau > 0) {
        tau = ParabolicInterpolation(cmndf, tau);
        frequency_ = GetPitchFromLag(tau + (float)minTau_);
    }
    else {
        frequency_ = -1.f;
    }
    return frequency_;
}
