// filters.h
#ifndef FILTERS_H
#define FILTERS_H

#include "daisy_seed.h"
#include "daisy_petal.h"
#include "daisysp.h"
#include "controls.h"

using namespace daisy;
using namespace daisysp;
using namespace daisy::seed;

// --- Global Hardware Objects ---
extern DaisySeed hw;

enum class ShelfType {
    LOW_SHELF,
    HIGH_SHELF
};

class ShelfFilter
{
public:
    ShelfFilter() {}

    void Init(float samplerate, float freq, ShelfType shelfType);

    float ProcessFilter(float in);

    void SetFreq(float freq);
    void SetGain(float gainDb);
    void SetQfactor(float Q);
    void SetOrder(int n); // 1 or 2
    void ComputeCoefficients();
    void ComputeLowShelf();
    void ComputeHighShelf();
    void PrintDebug();

private:
    // Coefficients
    float b0, b1, b2;
    float a0, a1, a2;

    // Coefficients
    float bz0, bz1, bz2;
    float az0, az1, az2;
    
    // Delay states (x[n-1], x[n-2], y[n-1], y[n-2])
    float x1 = 0.0f, x2 = 0.0f;  // Input delay states
    float y1 = 0.0f, y2 = 0.0f;  // Output delay states

    // Parameters
    float sampleRate = 48000.0f;
    float f0 = 20.f;
    float omegaC = 0.0f;
    float omega0 = 0.0f;
    float K = 0;
    float gain;
    float gainDb = -24.f;
    float gainSign = -1.f;
    float Q = 0.707f;
    int order = 2;

    ShelfType filterType;

    // Debug
    int printCounter = 0;
};

extern ShelfFilter lowShelf[2], highShelf[2];

void SetupFilters(float samplerate);
void UpdateFilters(float knobLow, SwitchState gainSwitch, SwitchState Qswitch);
float ProcessFilters(float inputSample, int channel);

#endif // FILTERS_H