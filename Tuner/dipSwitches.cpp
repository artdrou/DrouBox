#include "dipSwitches.h"
#include "daisy_seed.h"
using namespace daisy;


void DipManager::Init(dsy_gpio_pin pin1, dsy_gpio_pin pin2, dsy_gpio_pin pin3, dsy_gpio_pin pin4) {
    dsy_gpio_pin dipsPins[4] = {
        pin1,
        pin2,
        pin3,
        pin4
    };
    for (int i = 0; i < 4; i++) {
        dips[i].pin  = dipsPins[i];
        dips[i].mode = DSY_GPIO_MODE_INPUT;
        dips[i].pull = DSY_GPIO_PULLUP;
        dsy_gpio_init(&dips[i]);
    }
}

int DipManager::GetValue() {
    int value = 0; // reset each time
    value |= (!dsy_gpio_read(&dips[0])) << 3;
    value |= (!dsy_gpio_read(&dips[1])) << 2;
    value |= (!dsy_gpio_read(&dips[2])) << 1;
    value |= (!dsy_gpio_read(&dips[3])) << 0;
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
