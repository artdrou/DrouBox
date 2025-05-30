#include "metronome.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

float clickAmplitude = 0.1f;

void MetronomeClick::Init(float sample_rate)
{
    sample_rate_ = sample_rate;
    phase_ = 0.0f;
    env_ = 0.0f;
    freq_ = 1000.0f;
    env_phase_ = 1.0f;
    env_inc_ = 1.0f;
    active_ = false;
    SetDuration(20.0f);
}

void MetronomeClick::SetDuration(float ms)
{
    env_inc_ = 1.0f / (sample_rate_ * ms * 0.001f); // ms to samples
}

void MetronomeClick::Trigger(ClickType type)
{
    active_ = true;
    env_phase_ = 0.0f;

    if(type == ClickType::Accent)
    {
        freq_ = 1600.0f;
        amplitude_ = 1.25f*clickAmplitude;
    }
    else
    {
        freq_ = 1000.0f;
        amplitude_ = clickAmplitude;
    }
}

float MetronomeClick::Process()
{
    if(!active_)
        return 0.0f;

    float window = 0.5f * (1.0f - cosf(2.0f * M_PI * env_phase_)); // Hanning window
    float sample = amplitude_ * window * sinf(2.0f * M_PI * freq_ * phase_);

    phase_ += 1.0f / sample_rate_;
    if(phase_ >= 1.0f)
        phase_ -= 1.0f;

    env_phase_ += env_inc_;
    if(env_phase_ >= 1.0f)
        active_ = false;

    return sample;
}
