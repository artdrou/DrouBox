#include "controlMapper.h"
#include "utils/mapping.h"


Tuner::effectParams ControlMapper::MapTuner() const {
    Tuner::effectParams p;
    controls.Update();
    p.bypass = controls.GetFootswitch(0).GetState();
    return p;
}