#include "dipSwitches.h"
#include "daisy_seed.h"
using namespace daisy;


void DipManager::Init(dsy_gpio_pin pins[], size_t n) {
    nDips = n;
    dips = new dsy_gpio[nDips];
    for (int i = 0; i < nDips; i++) {
        dips[i].pin  = pins[i];
        dips[i].mode = DSY_GPIO_MODE_INPUT;
        dips[i].pull = DSY_GPIO_PULLUP;
        dsy_gpio_init(&dips[i]);
    }
}

int DipManager::GetValue() {
    int value = 0; // reset each time
    for (int i = 0; i < nDips; i++) {
        value |= (!dsy_gpio_read(&dips[i])) << (nDips-(i+1));
    }
    return value;
}

int DipManager::GetLastValue() const { return lastValue; }

bool DipManager::HasChanged() {
    int current = GetValue();
    if(current != lastValue) {
        lastValue = current;
        return true;
    }
    return false;
}
