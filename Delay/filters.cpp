#include "filters.h"
#include <utils/mapping.h>
#include "controls.h"
#include <cmath>
#define PI 3.1415926535897932384626433f

ShelfFilter lowShelf[2], highShelf[2];

void ShelfFilter::Init(float samplerate, float freq, ShelfType shelfType)
{
    sampleRate = samplerate;
    filterType = shelfType;
    SetFreq(freq);
    SetGain(gainDb);
    SetOrder(order);
}

void ShelfFilter::SetFreq(float freq)
{
    f0 = freq;
    omega0 = 2.0f * PI * f0;
    K = omega0 / (tan(omega0/(2*sampleRate)));
    ComputeCoefficients();
}

void ShelfFilter::SetGain(float gainDb)
{
    gain = pow(10.0f, abs(gainDb) / 20.0f);
    if (gainDb >= 0)
        gainSign = 1.f;
    else
        gainSign = -1.f;
    ComputeCoefficients();
}

void ShelfFilter::SetQfactor(float q)
{
    Q = q;
    ComputeCoefficients();
}

void ShelfFilter::SetOrder(int n)
{
    order = n;
    ComputeCoefficients();
}

void ShelfFilter::ComputeCoefficients()
{
    if (filterType == ShelfType::LOW_SHELF) {
        ComputeLowShelf();
    }
    else if (filterType == ShelfType::HIGH_SHELF) {
        ComputeHighShelf();
    }
}

void ShelfFilter::ComputeLowShelf()
{
    // First-order coefficients:
    if (order == 1)
    {
        b2 = 0.0f;
        b1 = 1.0f / omega0;
        b0 = pow(gain, gainSign*0.5f);
    
        a2 = 0.0f;
        a1 = 1.0f / omega0;
        a0 = pow(gain, -gainSign*0.5f);
        

        bz0 = (b1*K + b0)/(a1*K + a0);
        bz1 = (-b1*K + b0)/(a1*K + a0);
        bz2 = 0;
        az1 = (-a1*K + a0)/(a1*K + a0);
        az2 = 0;
    }
    else if (order == 2) // Second-order placeholders:
    {
        b2 = 1.0f/pow(omega0, 2);
        b1 = pow(gain, gainSign*0.25f) / (Q*omega0);
        b0 = pow(gain, gainSign*0.5f);
    
        a2 = 1.0f/pow(omega0, 2);
        a1 = pow(gain, -gainSign*0.25f) / (Q*omega0);
        a0 = pow(gain, -gainSign*0.5f);

        bz0 = (b2*pow(K, 2) + b1*K + b0)/(a2*pow(K, 2) + a1*K + a0);
        bz1 = (2*b0 - 2*b2*pow(K, 2))/(a2*pow(K, 2) + a1*K + a0);
        bz2 = (b2*pow(K, 2) - b1*K + b0)/(a2*pow(K, 2) + a1*K + a0);
        az1 = (2*a0 - 2*a2*pow(K, 2))/(a2*pow(K, 2) + a1*K + a0);
        az2 = (a2*pow(K, 2) - a1*K + a0)/(a2*pow(K, 2) + a1*K + a0);
    }
}

void ShelfFilter::ComputeHighShelf()
{
    // First-order coefficients:
    if (order == 1)
    {
        b2 = 0.0f;
        b1 = pow(gain, gainSign*0.5f) / omega0;
        b0 = 1.0f;
    
        a2 = 0.0f;
        a1 = pow(gain, -gainSign*0.5f) / omega0;
        a0 = 1.0f;

        bz0 = (b1*K + b0)/(a1*K + a0);
        bz1 = (-b1*K + b0)/(a1*K + a0);
        bz2 = 0;
        az1 = (-a1*K + a0)/(a1*K + a0);
        az2 = 0;
    }
    else if (order == 2) // Second-order placeholders:
    {
        b2 = pow(gain, gainSign*0.5f)/pow(omega0, 2);
        b1 = pow(gain, gainSign*0.25f) / (Q*omega0);
        b0 = 1.0f;
    
        a2 = pow(gain, -gainSign*0.5f)/pow(omega0, 2);
        a1 = pow(gain, -gainSign*0.25f) / (Q*omega0);
        a0 = 1.0f;

        bz0 = (b2*pow(K, 2) + b1*K + b0)/(a2*pow(K, 2) + a1*K + a0);
        bz1 = (2*b0 - 2*b2*pow(K, 2))/(a2*pow(K, 2) + a1*K + a0);
        bz2 = (b2*pow(K, 2) - b1*K + b0)/(a2*pow(K, 2) + a1*K + a0);
        az1 = (2*a0 - 2*a2*pow(K, 2))/(a2*pow(K, 2) + a1*K + a0);
        az2 = (a2*pow(K, 2) - a1*K + a0)/(a2*pow(K, 2) + a1*K + a0);
    }
}

float ShelfFilter::ProcessFilter(float in)
{
    // First-order: y[n] = b0*x[n] + b1*x[n-1] + b2*x[n-2] - a1*y[n-1] - a2*y[n-2]
    float out = bz0 * in + bz1 * x1 + bz2 * x2 - az1 * y1 - az2 * y2;

    // Update delay lines (for next sample)
    x2 = x1;
    x1 = in;
    y2 = y1;
    y1 = out;
    return out;
}

void ShelfFilter::PrintDebug()
{
    int testlowFreq = int(1*f0);
    hw.PrintLine("f0 = %d", testlowFreq);
}

void SetupFilters(float samplerate)
{
    for (int c = 0; c < 2; ++c)
    {
        lowShelf[c].Init(samplerate, 40.0f, ShelfType::LOW_SHELF);
        highShelf[c].Init(samplerate, 18000.0f, ShelfType::HIGH_SHELF);
    }
}

void UpdateFilters(float knobTone, SwitchState gainSwitch, SwitchState Qswitch)
{
    float lowFreq = logMap(knobTone, 0.5f, 1.f, 20.f, 500.f);
    float highFreq = logMap(knobTone, 0.f, 0.5f, 1000.f, 20000.f);
    float gaindB;
    float Q;
    if (gainSwitch == SwitchState::LEFT)
        gaindB = -36.f;
    else if (gainSwitch == SwitchState::OFF)
        gaindB = -24.f;
    else
        gaindB = -12.f;
    if (Qswitch == SwitchState::LEFT)
        Q = 1.f/sqrt(2.f);
    else if (Qswitch == SwitchState::OFF)
        Q = 1.f;
    else
        Q = 1.5f;
    for (int c = 0; c < 2; ++c)
    {
        lowShelf[c].SetFreq(lowFreq);
        lowShelf[c].SetGain(gaindB);
        lowShelf[c].SetQfactor(Q);
        highShelf[c].SetFreq(highFreq);
        highShelf[c].SetGain(gaindB);
        highShelf[c].SetQfactor(Q);
    }
}

float ProcessFilters(float inputSample, int channel)
{
    float out = lowShelf[channel].ProcessFilter(inputSample);
    out = highShelf[channel].ProcessFilter(out);
    return out;
}
