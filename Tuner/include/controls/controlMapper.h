#pragma once

#include "controls.h"
#include "gain.h"


class ControlMapper {
public:
    ControlMapper(Controls& hw) : controls(hw) {}

    Gain::effectParams MapGain() const;

private:
    Controls& controls;
};