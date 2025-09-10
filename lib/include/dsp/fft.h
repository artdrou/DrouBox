#pragma once
#include <vector>
#include <complex>
#include <cstddef>

// Naive Discrete Fourier Transform
std::vector<std::complex<float>> computeDFT(const std::vector<float>& input);

// Fast Fourier Transform (radix-2 Cooley–Tukey)
// Calls computeDFT if size is not power of 2
std::vector<std::complex<float>> computeFFT(const std::vector<float>& input);
