#include "daisy_seed.h"
#include "daisy_petal.h"
#include <vector>

class FootswitchManager {
public:
    void Init(daisy::Pin pin);
    void Update();
    bool Pressed();
    bool Released();
    float Held();
    bool GetState();
private:
    daisy::Switch footswitch;
    bool state = false;
};