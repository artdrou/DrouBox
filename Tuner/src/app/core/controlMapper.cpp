#include "controlMapper.h"
#include "utils/mapping.h"


Gain::effectParams ControlMapper::MapGain() const {
    Gain::effectParams p;

    controls.Update();
    p.gain = controls.GetKnobs().GetValue(0);
    p.bypass = controls.GetFootswitch(0).GetState();
    return p;
}

Tuner::effectParams ControlMapper::MapTuner() const {
    Tuner::effectParams p;

    controls.Update();
    p.sampleRate = controls.GetHwPtr()->AudioSampleRate();
    p.smoothedFreq = 400.f*controls.GetKnobs().GetValue(0);
    p.bypass = controls.GetFootswitch(0).GetState();
    return p;
}