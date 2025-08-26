#pragma once

#include "controls.h"
#include "tuner.h"


class ControlMapper {
public:
    ControlMapper(Controls& hw) : controls(hw) {}

    Tuner::effectParams MapTuner() const;

    // getter for controls
    Controls& GetControls() { return controls; }
    const Controls& GetControls() const { return controls; }

private:
    Controls& controls;
};