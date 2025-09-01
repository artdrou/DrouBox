#include "logging.h"


HardwareLogger::HardwareLogger(Controls& controlsRef, DaisySeed& hwRef)
    : controls(controlsRef), hw(hwRef)
{}

void HardwareLogger::TestHardware() {
    // Footswitches
    for (size_t i = 0; i < controls.NumFootswitches(); ++i) {
        controls.GetFootswitch(i).Update();
        if (controls.GetFootswitch(i).Pressed()) {
            hw.PrintLine("Footswitch %d Pressed -> %d", i+1, controls.GetFootswitch(i).GetState());
        }
    }

    // LEDs
    for (size_t i = 0; i < controls.NumLeds(); ++i) {
        controls.GetLed(i).Set(controls.GetFootswitch(i).GetState());
    }

    // Dips
    for (size_t i = 0; i < controls.NumDips(); ++i) {
        if (controls.GetDips(i).HasChanged()) {
            hw.PrintLine("Dips value has changed -> %d", controls.GetDips(i).GetValue());
        }
    }

    // Switches
    for (size_t i = 0; i < controls.NumSwitches(); ++i) {
        if (controls.GetSwitch(i).HasChanged()) {
            hw.PrintLine("Switch %d value has changed -> %s",
                         i+1,
                         controls.GetSwitch(i).ToString(controls.GetSwitch(i).GetState()));
        }
    }

    // Knobs
    controls.GetKnobs().Update();
    for (size_t i = 0; i < controls.NumKnobs(); ++i) {
        if (controls.GetKnobs().HasChanged(i)) {
            hw.PrintLine("Knob %d value has changed -> %d",
                         i+1,
                         (int)(1000.f * controls.GetKnobs().GetValue(i)));
        }
    }
}
