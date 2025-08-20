#pragma once

#include "daisy_seed.h"

class LedManager {
public:
    void Init(Pin pin) {
        led.Init(pin, false, 1000);
    }
    void Update() {
        led.Update();
    }

    void Set(bool state) {
        ledState = state;
        led.Set(ledState);
        this->Update();
    }

    void Toggle() {
        Set(!ledState);
        this->Update();
    }

    bool GetState() const {
        return ledState;
    }

    void InitBlinking(uint32_t intervalMs, int totalBlinks) {
        blinkInterval = intervalMs;
        blinkTarget   = totalBlinks;
        blinkCount    = 0;
        blinking      = true;
        lastBlinkTime = System::GetNow();
        blinkState    = false;
    }

    void StopBlinking() {
        blinking = false;
        blinkCount = 0;
        blinkTarget = -1;
        Set(false);
    }

    void HandleBlink() {
        if(!blinking) return;
        uint32_t now = System::GetNow();
        if (now - lastBlinkTime >= blinkInterval) {
            blinkState = !blinkState;
            Set(blinkState);
            lastBlinkTime = now;
            if(blinkTarget > 0 && !blinkState) {
                blinkCount++;
                // if(blinkCount >= blinkTarget) {
                //     StopBlinking();
                // }
            }
        }
    }

private:
    daisy::Led led;
    bool ledState = false;

    // blinking
    bool blinking = false;
    bool blinkState = false;
    int blinkCount = 0;
    int blinkTarget = -1;
    uint32_t blinkInterval = 100;
    uint32_t lastBlinkTime = 0;
};