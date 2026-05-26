#pragma once

namespace droubox {

// First-order DC blocking filter: y[n] = x[n] - x[n-1] + R*y[n-1]
// R close to 1 gives a very low cutoff (~5 Hz at 48 kHz).
class DcBlock {
public:
    explicit DcBlock(float r = 0.995f) : r_(r) {}
    float Process(float in) {
        float out = in - xPrev_ + r_ * yPrev_;
        xPrev_ = in;
        yPrev_ = out;
        return out;
    }
    void Reset() { xPrev_ = yPrev_ = 0.f; }

private:
    float r_     = 0.995f;
    float xPrev_ = 0.f;
    float yPrev_ = 0.f;
};

} // namespace droubox
