#pragma once
#include <cstdint>

namespace daisy {

struct Pin { int id = 0; };

namespace System {
    inline uint32_t& _now() { static uint32_t t = 0; return t; }
    inline uint32_t GetNow() { return _now(); }
    inline void AdvanceMs(uint32_t ms) { _now() += ms; }
    inline void ResetClock() { _now() = 0; }
}

class Switch {
public:
    void Init(Pin, uint32_t) {}
    void Debounce() {}
    bool RisingEdge()  const { return rising_; }
    bool FallingEdge() const { return falling_; }
    bool Pressed()     const { return pressed_; }
    float TimeHeldMs() const { return heldMs_; }

    void SimPress()                { rising_ = true;  falling_ = false; pressed_ = true;  heldMs_ = 0; }
    void SimHold(float ms)         { rising_ = false; falling_ = false; pressed_ = true;  heldMs_ = ms; }
    void SimRelease(float heldMs)  { rising_ = false; falling_ = true;  pressed_ = false; heldMs_ = heldMs; }
    void SimIdle()                 { rising_ = false; falling_ = false; }

private:
    bool  rising_  = false;
    bool  falling_ = false;
    bool  pressed_ = false;
    float heldMs_  = 0;
};

} // namespace daisy
