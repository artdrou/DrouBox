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

float Yin::CubicInterpolation(const std::vector<float>& cmndf, float tau) {
    int N = static_cast<int>(cmndf.size());
    if (tau <= 0 || tau >= N - 3) {
        return tau; // fallback to original if near edges
    }

    // values
    float y0 = cmndf[(int)tau - 1];
    float y1 = cmndf[(int)tau];
    float y2 = cmndf[(int)tau + 1];
    float y3 = cmndf[(int)tau + 2];

    // cubic coefficients for points x = 0,1,2,3
    // using Lagrange interpolation simplified
    float a = (-y0 + 3*y1 - 3*y2 + y3) / 6.0f;
    float b = (y0 - 2*y1 + y2) / 2.0f;
    float c = (-11*y0 + 18*y1 - 9*y2 + 2*y3) / 6.0f;
    float d = y1;

    // derivative dy/dx = 3ax^2 + 2bx + c = 0
    float discrim = b*b - 3*a*c;
    float x_min = 0.0f;
    if (a == 0.0f) {
        if (b != 0.0f) x_min = -c / (2.0f*b);
        else x_min = 0.0f;
    } else if (discrim >= 0.0f) {
        float r1 = (-b + std::sqrt(discrim)) / (3.0f*a);
        float r2 = (-b - std::sqrt(discrim)) / (3.0f*a);
        // choose root inside [0,2] (tau..tau+2)
        if (r1 >= 0.0f && r1 <= 2.0f) x_min = r1;
        else if (r2 >= 0.0f && r2 <= 2.0f) x_min = r2;
        else x_min = 0.0f; // fallback
    }

    return tau + x_min - 1.0f; // shift because y0 = tau-1
}

float Yin::GetPitchFromLag(float tau) {
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
        // tau = ParabolicInterpolation(cmndf, tau);
        tau = CubicInterpolation(cmndf, tau);
        frequency_ = GetPitchFromLag(tau + (float)minTau_);
    }
    else {
        frequency_ = -1.f;
    }
    return frequency_;
}
