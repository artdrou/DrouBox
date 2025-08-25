#include "daisy_seed.h"
#include "daisy_petal.h"
#include "daisysp.h"

#include "controls.h"
#include "effectsManager.h"
#include "controlMapper.h"
#include "gain.h"


using namespace daisy;
using namespace daisysp;
using namespace daisy::seed;

DaisySeed hw;
Controls controls;
ControlMapper mapper(controls);

EffectManager effectManager;
Gain gainEffect;

// DEV ONLY REBOOT
bool reboot = false;

void rebootDfuUpdate() {
    
    if (controls.GetFootswitch(0).Held() > 3000) {
        reboot = true;
    }
    if (reboot) {
        controls.GetLed(0).BlinkFor(500, 50, System::GetNow());
        controls.GetLed(1).BlinkFor(500, 50, System::GetNow());
        if (!controls.GetLed(0).IsBlinking()) {
            System::ResetToBootloader(); // Restart in bootloader mode
            reboot = false;
        }
    }
}

void testHardware() {
    // Footswitches
    for (size_t i = 0; i < controls.NumFootswitches(); ++i) {
        controls.GetFootswitch(i).Update();
        if (controls.GetFootswitch(i).Pressed()) {
            hw.PrintLine("Footswitch %d Pressed -> %d", i+1, controls.GetFootswitch(i).GetState());
        }
    }

    // Leds
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
            hw.PrintLine("Switch %d value has changed -> %s", i+1, controls.GetSwitch(i).ToString(controls.GetSwitch(i).GetState()));
        }
    }

    // knobs
    controls.GetKnobs().Update();
    for (size_t i = 0; i < controls.NumKnobs(); ++i) {
        if (controls.GetKnobs().HasChanged(i)) {
            hw.PrintLine("knob %d value has changed -> %d", i+1, (int)(1000.f*controls.GetKnobs().GetValue(i)));
        }
    }
}

// AUDIO CALLBACK =========================================================================

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
    effectManager.Process(in[0], out[0], size);
}



// MAIN ===================================================================================

int main(void)
{
    hw.Init(true);
    hw.SetAudioBlockSize(1024); // Number of samples handled per callback
    hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);

    // AUDIO
    hw.StartAudio(AudioCallback);
    hw.StartLog(false);

    // CONTROLS
    controls.Init(hw);

    // Effects
    effectManager.AddEffect(&gainEffect);

    while (1)
    {
        effectManager.UpdateParameters(mapper);
        effectManager.UpdateUI(controls);
        // REBOOT BOARD DEV ONLY !!!!!!!!
        rebootDfuUpdate();

        System::Delay(10);
    }
}
