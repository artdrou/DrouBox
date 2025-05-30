#ifndef METRONOME_H
#define METRONOME_H

enum class ClickType
{
    Normal,
    Accent
};

class MetronomeClick
{
  public:
    void Init(float sample_rate);
    void Trigger(ClickType type = ClickType::Normal);
    float Process();
    void SetDuration(float ms); // Click length in milliseconds

  private:
    float sample_rate_;
    float phase_;
    float env_;
    float freq_;
    float amplitude_;
    float env_phase_;     // 0 to 1 across click duration
    float env_inc_;       // how much env_phase advances each sample
    bool active_;
};
#endif // METRONOME_H