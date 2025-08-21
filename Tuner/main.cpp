#include "daisy_seed.h"
#include "daisy_petal.h"
#include "daisysp.h"
#include "leds.h"
#include "dipSwitches.h"
#include "footSwitchs.h"
#include "onOffOnSwitches.h"
#include "knobs.h"
#include <utils/mapping.h>

using namespace daisy;
using namespace daisysp;
using namespace daisy::seed;

DaisySeed hw;

// Global declaration because stack overflowed when declared in main
FootswitchManager fsw1, fsw2;
LedManager ledMgr1, ledMgr2;
DipManager dips;
OnOffOnSwitchManager sw1;
// KnobsManager knobMgr;
KnobsManager* knobMgr = new KnobsManager;

dsy_gpio_pin knobPins[1] = { A1 };

// DFU reboot (DEV ONLY)
bool reboot = false;
int dipsValue = 0;
float knobValue = 0.f;


void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        out[0][i] = in[0][i];
    }
}

int main(void)
{
    hw.Configure();
    hw.Init(true);
    hw.SetAudioBlockSize(512); // Number of samples handled per callback
    hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);

    // LEDS
    ledMgr1.Init(A7);
    ledMgr2.Init(A8);

    // FOOTSWITCH
    fsw1.Init(D25);
    fsw2.Init(D26);

    // DIPS
    dips.Init(
        hw.GetPin(1),
        hw.GetPin(3),
        hw.GetPin(5),
        hw.GetPin(6)
    );

    // ON-OFF-ON SWITCHES
    sw1.Init(hw.GetPin(14), hw.GetPin(13));
    
    // KNOBS
    // knobMgr.Init(hw, knobPins, 1);
    knobMgr->Init(hw, knobPins, 1);
    // AUDIO
    hw.StartAudio(AudioCallback);
    hw.StartLog(false);
    while (1)
    {
        // fOOTSWITCH
        fsw1.Update();
        fsw2.Update();
        if (fsw1.Held() > 3000) {
            reboot = true;
        }
        if (reboot) {
            ledMgr1.BlinkFor(500, 50, System::GetNow());
            ledMgr2.BlinkFor(500, 50, System::GetNow());
            if (!ledMgr1.IsBlinking()) {
                System::ResetToBootloader(); // Restart in bootloader mode
                reboot = false;
            }
        }


        // LEDS
        ledMgr1.Set(fsw1.GetState());
        ledMgr2.Set(fsw2.GetState());

        // DIPS
        if (dips.HasChanged()) {
            dipsValue = dips.GetValue();
            hw.PrintLine("Changed! New value: %d", dipsValue);
        }

        // ON-OFF-ON SWITCHES
        if(sw1.HasChanged())
        {
            hw.PrintLine("Switch state changed: %s", sw1.ToString(sw1.GetState()));
        }

        // KNOBS
        knobMgr->Update();
        if (knobMgr->HasChanged(0, 0.001f)) {
            knobValue = knobMgr->GetValue(0);
            hw.PrintLine("Knob value has changed: %d", (int)(100.f*knobValue));
        }
        
        System::Delay(10);
    }
}
