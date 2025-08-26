#include "controlMapper.h"
#include "utils/mapping.h"


Tuner::effectParams ControlMapper::MapTuner() const {
    Tuner::effectParams p;
    controls.Update();
    p.sampleRate = controls.GetHwPtr()->AudioSampleRate();
    p.frequency = (int)(400.f*controls.GetKnobs().GetValue(0));
    p.bypass = controls.GetFootswitch(0).GetState();
    return p;
}