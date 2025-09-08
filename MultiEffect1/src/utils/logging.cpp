#include "logging.h"
#include "footSwitchPair.h"


HardwareLogger::HardwareLogger(Controls& controlsRef, DaisySeed& hwRef)
    : controls_(controlsRef), hw(hwRef)
{}

void HardwareLogger::TestHardware() {
    // Assume we have 2 footswitches
    FootswitchManager& fs1 = controls_.GetFootswitch(0);
    FootswitchManager& fs2 = controls_.GetFootswitch(1);

    // Create the pair wrapper (can also be a member instead of local)
    static FootswitchPair pair(&fs1, &fs2);

    // Update once per cycle
    pair.Update();

    // 1️⃣ check BOTH first
    if (pair.BothTapped(1000)) {
        hw.PrintLine("Both footswitches tapped");
    } else if (pair.ATap(1000)) {
        hw.PrintLine("Footswitch 1 tapped");
    } else if (pair.BTap(1000)) {
        hw.PrintLine("Footswitch 2 tapped");
    }
    
    // 2️⃣ same for held
    if (pair.BothHeldForTrigger(1.0f)) {
        hw.PrintLine("Both footswitches held for 1s");
    } else if (pair.AHeld(1.0f)) {
        hw.PrintLine("Footswitch 1 held for 1s");
    } else if (pair.BHeld(1.0f)) {
        hw.PrintLine("Footswitch 2 held for 1s");
    }


    // ==============================================================================================
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
