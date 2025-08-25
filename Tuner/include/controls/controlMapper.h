#pragma once

#include "controls.h"
#include "gain.h"
#include "tuner.h"


class ControlMapper {
public:
    ControlMapper(Controls& hw) : controls(hw) {}

    Gain::effectParams MapGain() const;
    Tuner::effectParams MapTuner() const;

private:
    Controls& controls;
};