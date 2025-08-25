#include "daisy_seed.h"
#include "daisy_petal.h"
#include "daisysp.h"

#include "controls.h"
#include "effectsManager.h"
#include "controlMapper.h"
#include "tuner.h"

#include "logging.h"


using namespace daisy;
using namespace daisysp;
using namespace daisy::seed;

DaisySeed hw;
Controls controls;
ControlMapper mapper(controls);

EffectManager effectManager;
Tuner tuner;

// DEV ONLY REBOOT & LOG
HardwareLogger logger(controls, hw);
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
    effectManager.AddEffect(&tuner);

    while (1)
    {
        // logger.TestHardware();

        effectManager.UpdateParameters(mapper);
        effectManager.UpdateUI(controls);

        // // REBOOT BOARD DEV ONLY !!!!!!!!
        rebootDfuUpdate();

        System::Delay(10);
    }
}
