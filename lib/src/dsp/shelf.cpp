#include "constants.h"
#include "shelf.h"
#include "mapping.h"
#include <cmath>


using namespace drouAudioLib;

void ShelfFilter::init(float samplerate, float freq, ShelfType shelfType, float gainDb, int order)
{
    sampleRate_ = samplerate;
    filterType_ = shelfType;
    setFreq(freq);
    setGain(gainDb);
    setOrder(order);
}

void ShelfFilter::setFreq(float freq)
{
    f0_ = freq;
    omega0_ = TWO_PI * f0_;
    K_ = omega0_ / (tan(omega0_/(2*sampleRate_)));
    computeCoefficients();
}

void ShelfFilter::setGain(float gainDb)
{
    gain_ = pow(10.0f, abs(gainDb) / 20.0f);
    if (gainDb >= 0)
        gainSign_ = 1.f;
    else
        gainSign_ = -1.f;
    computeCoefficients();
}

void ShelfFilter::setQfactor(float q)
{
    Q_ = q;
    computeCoefficients();
}

void ShelfFilter::setOrder(int n)
{
    order_ = n;
    computeCoefficients();
}

void ShelfFilter::computeCoefficients()
{
    if (filterType_ == ShelfType::LOW_SHELF) {
        computeLowShelf();
    }
    else if (filterType_ == ShelfType::HIGH_SHELF) {
        computeHighShelf();
    }
}

void ShelfFilter::computeLowShelf()
{
    // First-order coefficients:
    if (order_ == 1)
    {
        b2_ = 0.0f;
        b1_ = 1.0f / omega0_;
        b0_ = pow(gain_, gainSign_*0.5f);
    
        a2_ = 0.0f;
        a1_ = 1.0f / omega0_;
        a0_ = pow(gain_, -gainSign_*0.5f);
        

        bz0_ = (b1_*K_ + b0_)/(a1_*K_ + a0_);
        bz1_ = (-b1_*K_ + b0_)/(a1_*K_ + a0_);
        bz2_ = 0;
        az1_ = (-a1_*K_ + a0_)/(a1_*K_ + a0_);
        az2_ = 0;
    }
    else if (order_ == 2) // Second-order placeholders:
    {
        b2_ = 1.0f/pow(omega0_, 2);
        b1_ = pow(gain_, gainSign_*0.25f) / (Q_*omega0_);
        b0_ = pow(gain_, gainSign_*0.5f);
    
        a2_ = 1.0f/pow(omega0_, 2);
        a1_ = pow(gain_, -gainSign_*0.25f) / (Q_*omega0_);
        a0_ = pow(gain_, -gainSign_*0.5f);

        bz0_ = (b2_*pow(K_, 2) + b1_*K_ + b0_)/(a2_*pow(K_, 2) + a1_*K_ + a0_);
        bz1_ = (2*b0_ - 2*b2_*pow(K_, 2))/(a2_*pow(K_, 2) + a1_*K_ + a0_);
        bz2_ = (b2_*pow(K_, 2) - b1_*K_ + b0_)/(a2_*pow(K_, 2) + a1_*K_ + a0_);
        az1_ = (2*a0_ - 2*a2_*pow(K_, 2))/(a2_*pow(K_, 2) + a1_*K_ + a0_);
        az2_ = (a2_*pow(K_, 2) - a1_*K_ + a0_)/(a2_*pow(K_, 2) + a1_*K_ + a0_);
    }
}

void ShelfFilter::computeHighShelf()
{
    // First-order coefficients:
    if (order_ == 1)
    {
        b2_ = 0.0f;
        b1_ = pow(gain_, gainSign_*0.5f) / omega0_;
        b0_ = 1.0f;
    
        a2_ = 0.0f;
        a1_ = pow(gain_, -gainSign_*0.5f) / omega0_;
        a0_ = 1.0f;

        bz0_ = (b1_*K_ + b0_)/(a1_*K_ + a0_);
        bz1_ = (-b1_*K_ + b0_)/(a1_*K_ + a0_);
        bz2_ = 0;
        az1_ = (-a1_*K_ + a0_)/(a1_*K_ + a0_);
        az2_ = 0;
    }
    else if (order_ == 2) // Second-order placeholders:
    {
        b2_ = pow(gain_, gainSign_*0.5f)/pow(omega0_, 2);
        b1_ = pow(gain_, gainSign_*0.25f) / (Q_*omega0_);
        b0_ = 1.0f;
    
        a2_ = pow(gain_, -gainSign_*0.5f)/pow(omega0_, 2);
        a1_ = pow(gain_, -gainSign_*0.25f) / (Q_*omega0_);
        a0_ = 1.0f;

        bz0_ = (b2_*pow(K_, 2) + b1_*K_ + b0_)/(a2_*pow(K_, 2) + a1_*K_ + a0_);
        bz1_ = (2*b0_ - 2*b2_*pow(K_, 2))/(a2_*pow(K_, 2) + a1_*K_ + a0_);
        bz2_ = (b2_*pow(K_, 2) - b1_*K_ + b0_)/(a2_*pow(K_, 2) + a1_*K_ + a0_);
        az1_ = (2*a0_ - 2*a2_*pow(K_, 2))/(a2_*pow(K_, 2) + a1_*K_ + a0_);
        az2_ = (a2_*pow(K_, 2) - a1_*K_ + a0_)/(a2_*pow(K_, 2) + a1_*K_ + a0_);
    }
}

float ShelfFilter::processFilter(float in)
{
    // First-order: y[n] = b0_*x[n] + b1_*x[n-1] + b2_*x[n-2] - a1_*y[n-1] - a2_*y[n-2]
    float out = bz0_ * in + bz1_ * x1_ + bz2_ * x2_ - az1_ * y1_ - az2_ * y2_;

    // Update delay lines (for next sample)
    x2_ = x1_;
    x1_ = in;
    y2_ = y1_;
    y1_ = out;
    return out;
}
