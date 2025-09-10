#pragma once

namespace drouAudioLib {
enum class ShelfType {
    LOW_SHELF,
    HIGH_SHELF
};

class ShelfFilter
{
public:
    ShelfFilter() {}

    void init(float samplerate, float freq, ShelfType shelfType, float gainDb, int order);

    float processFilter(float in);

    void setFreq(float freq);
    void setGain(float gainDb);
    void setQfactor(float Q);
    void setOrder(int n); // 1 or 2
    void computeCoefficients();
    void computeLowShelf();
    void computeHighShelf();

private:
    // Coefficients
    float b0_, b1_, b2_;
    float a0_, a1_, a2_;

    // Normalized coefficients
    float bz0_, bz1_, bz2_;
    float az0_, az1_, az2_;
    
    // Delay states (x[n-1], x[n-2], y[n-1], y[n-2])
    float x1_ = 0.0f, x2_ = 0.0f;  // Input delay states
    float y1_ = 0.0f, y2_ = 0.0f;  // Output delay states

    // Parameters member variables
    float sampleRate_ = 48000.0f;
    float f0_ = 20.f;
    float omegaC_ = 0.0f;
    float omega0_ = 0.0f;
    float K_ = 0;
    float gain_;
    float gainDb_ = -24.f;
    float gainSign_ = -1.f;
    float Q_ = 0.707f;
    int order_ = 2;

    ShelfType filterType_ = ShelfType::LOW_SHELF;
};
};