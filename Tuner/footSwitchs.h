#include "daisy_seed.h"
#include "daisy_petal.h"
#include <vector>

class FootswitchManager {
public:
    void Init(daisy::Pin pin) {
        footswitch.Init(pin, 1000);
    }
    void Update() {
        footswitch.Debounce();
        if (this->Pressed()) {
            state = !state;
        }
    }
    bool Pressed() {
        return footswitch.RisingEdge();
    }
    bool Released() {return footswitch.FallingEdge();}
    float Held() {return footswitch.TimeHeldMs();}
    bool GetState() {return state;}

private:
    daisy::Switch footswitch;
    bool state = false;
};