#ifndef BPM_H
#define BPM_H

#include "daisy_seed.h"

class BpmRunner
{
  public:
    void Init(float bpm, int beatsPerMeasure = 4);
    void Update();
    bool IsBeat();              // True on new beat
    bool IsMeasure();           // True on first beat of measure
    int GetBeatInMeasure() const;
    void Reset();
    void SetBpm(float newBpm);
    float GetBpm();
    int GetBeatsPerMeasure();

  private:
    float bpm_;
    int beats_per_measure_;
    int current_beat_;
    uint32_t last_tick_time_;
    uint32_t interval_ms_;
    bool is_beat_;
    bool is_measure_;
};

#endif // BPM_H