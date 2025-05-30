#include "bpm.h"
#include "daisy_seed.h"

using namespace daisy;

void BpmRunner::Init(float bpm, int beatsPerMeasure)
{
    bpm_ = bpm;
    beats_per_measure_ = beatsPerMeasure;
    interval_ms_ = static_cast<uint32_t>(60000.0f / bpm_);
    last_tick_time_ = System::GetNow();
    current_beat_ = 0;
    is_beat_ = false;
    is_measure_ = false;
}

void BpmRunner::Update()
{
    uint32_t now = System::GetNow();
    is_beat_ = false;
    is_measure_ = false;

    if ((now - last_tick_time_) >= interval_ms_)
    {
        last_tick_time_ += interval_ms_;
        is_beat_ = true;

        current_beat_ = (current_beat_ + 1) % beats_per_measure_;
        is_measure_ = (current_beat_ == 0);
    }
}

bool BpmRunner::IsBeat()
{
    return is_beat_;
}

bool BpmRunner::IsMeasure()
{
    return is_measure_;
}

int BpmRunner::GetBeatInMeasure() const
{
    return current_beat_;
}

void BpmRunner::Reset()
{
    last_tick_time_ = System::GetNow();
    current_beat_ = 0;
}

void BpmRunner::SetBpm(float newBpm)
{
    bpm_ = newBpm;
    interval_ms_ = static_cast<uint32_t>(60000.0f / bpm_);
}
