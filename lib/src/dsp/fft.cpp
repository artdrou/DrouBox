#include "constants.h"
#include "fft.h"
#include <cmath>
#include <complex>
#include <algorithm>


std::vector<std::complex<float>> computeDFT(const std::vector<float>& input)
{
    const std::size_t N = input.size();
    std::vector<std::complex<float>> output(N);

    for (std::size_t k = 0; k < N; ++k) {
        std::complex<float> sum(0.0f, 0.0f);
        for (std::size_t n = 0; n < N; ++n) {
            float angle = -2.0f * static_cast<float>(PI) * k * n / static_cast<float>(N);
            sum += std::complex<float>(input[n], 0.0f) * std::complex<float>(std::cos(angle), std::sin(angle));
        }
        output[k] = sum;
    }

    return output;
}


// Utility: check if N is power of 2
static bool isPowerOfTwo(std::size_t n)
{
    return (n > 0) && ((n & (n - 1)) == 0);
}


// Fast FFT: Cooley–Tukey radix-2 iterative
// Falls back to DFT if N not power of 2
std::vector<std::complex<float>> computeFFT(const std::vector<float>& input)
{
    const std::size_t N = input.size();
    // If N not power of 2, fallback to DFT
    if (!isPowerOfTwo(N)) {
        return computeDFT(input);
    }
    // Convert input to complex
    std::vector<std::complex<float>> data(N);
    for (std::size_t i = 0; i < N; ++i) {
        data[i] = std::complex<float>(input[i], 0.0f);
    }
    // Bit-reversal permutation
    std::size_t j = 0;
    for (std::size_t i = 0; i < N; ++i) {
        if (i < j) std::swap(data[i], data[j]);
        std::size_t m = N >> 1;
        while (m > 0 && j >= m) {
            j -= m;
            m >>= 1;
        }
        j += m;
    }
    // Iterative FFT
    for (std::size_t len = 2; len <= N; len <<= 1) {
        float ang = -2.0f * static_cast<float>(PI) / static_cast<float>(len);
        std::complex<float> wlen(std::cos(ang), std::sin(ang));
        for (std::size_t i = 0; i < N; i += len) {
            std::complex<float> w(1.0f, 0.0f);
            for (std::size_t k = 0; k < len / 2; ++k) {
                std::complex<float> u = data[i + k];
                std::complex<float> v = data[i + k + len / 2] * w;
                data[i + k] = u + v;
                data[i + k + len / 2] = u - v;
                w *= wlen;
            }
        }
    }
    return data;
}
