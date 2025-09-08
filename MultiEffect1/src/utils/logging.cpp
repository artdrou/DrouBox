#include "logging.h"


HardwareLogger::HardwareLogger(Controls& controlsRef, DaisySeed& hwRef)
    : controls_(controlsRef), hw(hwRef)
{}

void HardwareLogger::TestHardware() {
    for (size_t i = 0; i < controls_.NumFootswitches(); ++i) {
        controls_.GetFootswitch(i).Update();
        if (controls_.GetFootswitch(i).IsTap(1000)) {
            hw.PrintLine("Footswitch %d tapped", i+1);
        };
        if (controls_.GetFootswitch(i).IsHeldForTrigger(1)) {
            hw.PrintLine("Footswitch %d held for 1s", i+1);
        };
    }

    if (controls_.GetFootswitch(0).IsTap(1000)  && controls_.GetFootswitch(1).IsTap(1000)) {
        hw.PrintLine("Both footswitch tapped");
    };
    if (controls_.GetFootswitch(0).IsHeldForTrigger(1) && controls_.GetFootswitch(1).IsHeldForTrigger(1)) {
        hw.PrintLine("Both footswitch held");
    };
    // // Footswitches
    // for (size_t i = 0; i < controls_.NumFootswitches(); ++i) {
    //     controls_.GetFootswitch(i).Update();
    //     if (controls_.GetFootswitch(i).Pressed()) {
    //         hw.PrintLine("Footswitch %d Pressed -> %d", i+1, controls_.GetFootswitch(i).GetState());
    //     }
    // }

    // LEDs
    for (size_t i = 0; i < controls_.NumLeds(); ++i) {
        controls_.GetLed(i).Set(controls_.GetFootswitch(i).GetState());
    }

    // Dips
    for (size_t i = 0; i < controls_.NumDips(); ++i) {
        if (controls_.GetDips(i).HasChanged()) {
            hw.PrintLine("Dips value has changed -> %d", controls_.GetDips(i).GetValue());
        }
    }

    // Switches
    for (size_t i = 0; i < controls_.NumSwitches(); ++i) {
        if (controls_.GetSwitch(i).HasChanged()) {
            hw.PrintLine("Switch %d value has changed -> %s",
                         i+1,
                         controls_.GetSwitch(i).ToString(controls_.GetSwitch(i).GetState()));
        }
    }

    // Knobs
    controls_.GetKnobs().Update();
    for (size_t i = 0; i < controls_.NumKnobs(); ++i) {
        if (controls_.GetKnobs().HasChanged(i)) {
            hw.PrintLine("Knob %d value has changed -> %d",
                         i+1,
                         (int)(1000.f * controls_.GetKnobs().GetValue(i)));
        }
    }
}
