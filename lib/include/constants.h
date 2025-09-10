#ifndef CONSTANTS_H
#define CONSTANTS_H


// Math constants
constexpr double PI       = 3.14159265358979323846;
constexpr double TWO_PI   = 6.28318530717958647692;
constexpr double HALF_PI  = 1.57079632679489661923;
constexpr double INV_PI   = 0.31830988618379067154; // 1/pi
constexpr double SQRT2    = 1.41421356237309504880;

// Reference tuning
constexpr double A4_FREQ = 440.0;

// Frequency ranges
constexpr double MIN_HEARING_FREQ = 20.0;     // Hz
constexpr double MAX_HEARING_FREQ = 20000.0;  // Hz

// Typical guitar range
constexpr double GUITAR_LOW_FREQ  = 82.41;    // E2
constexpr double GUITAR_HIGH_FREQ = 1318.51;  // E6


// DSP sampling defaults
constexpr int DEFAULT_SAMPLE_RATE = 48000;  // Hz
constexpr int DEFAULT_FFT_SIZE    = 1024;   // Must be power of 2

// Derived values
constexpr double DEFAULT_NYQUIST = DEFAULT_SAMPLE_RATE / 2.0;

#endif // CONSTANTS_H
