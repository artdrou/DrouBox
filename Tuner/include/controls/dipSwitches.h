#pragma once
#include "daisy_seed.h"
using namespace daisy;

class DipManager {
  public:
    void Init(dsy_gpio_pin pin1, dsy_gpio_pin pin2, dsy_gpio_pin pin3, dsy_gpio_pin pin4);
    int GetValue();
    bool HasChanged();
    int GetLastValue() const;
    

  private:
    dsy_gpio dips[4];
    int lastValue = -1;
};
