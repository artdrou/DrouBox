#include "leds.h"

void LedManager::Init(daisy::Pin pin) {
    led.Init(pin, false, 1000);
    // reset states properly
    ledState = false;
    blinking = false;
    blinkState = false;
    blinkCount = 0;
    blinkInterval = 100;
    lastBlinkTime = 0;
}

void LedManager::Update() {
    led.Update();
}

void LedManager::Set(bool state) {
    if (!blinking) {
        ledState = state;
        led.Set(ledState);
        led.Update();
    }
}

void LedManager::Set(float brightness) {
    if (!blinking) {
        if (brightness < 0.0f) brightness = 0.0f;
        if (brightness > 1.0f) brightness = 1.0f;
        ledState = (brightness > 0.0f);
        led.Set(brightness);
        led.Update();
    }
}

void LedManager::Toggle() {
    Set(!ledState);
}

bool LedManager::GetState() {
    return ledState;
}

void LedManager::StopBlinking() {
    blinking = false;
    blinkCount = 0;
    Set(false);
}

void LedManager::BlinkFor(float durationMs, uint32_t intervalMs, uint32_t now) {
    // Start blinking if not active
    if(!blinking) {
        blinkInterval = intervalMs;
        blinkCount    = 0;
        blinking      = true;
        lastBlinkTime = now;
        blinkState    = false;
    }

    // Perform blinking logic
    if (now - lastBlinkTime >= blinkInterval) {
        blinkState = !blinkState;
        led.Set(blinkState);
        led.Update();
        lastBlinkTime = now;
        if(durationMs > 0 && !blinkState) {
            blinkCount++;
            if(blinkCount >= (float)durationMs / blinkInterval) {
                StopBlinking();
            }
        }
    }
}

bool LedManager::IsBlinking() {return blinking;}