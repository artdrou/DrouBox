#pragma once
#include "daisy_seed.h"
using namespace daisy;

class DipManager {
  public:
    void Init(dsy_gpio_pin pins[], size_t n);
    int GetValue();
    bool HasChanged();
    int GetLastValue() const;
    
    ~DipManager() { delete[] dips; }

  private:
    dsy_gpio* dips = nullptr;
    int nDips;
    int lastValue = -1;
};
