#include "daisy_seed.h"
#include "daisy_petal.h"
#include "daisysp.h"

#include "controls.h"
#include "effectsManager.h"
#include "tuner.h"

#include "logging.h"


using namespace daisy;
using namespace daisysp;
using namespace daisy::seed;

#define BLOCK_SIZE 1024

DaisySeed hw;
Controls controls;

EffectManager effectManager(controls);
Tuner tuner(controls);

// DEV ONLY REBOOT & LOG
HardwareLogger logger(controls, hw);
bool reboot = false;

void rebootDfuUpdate(uint32_t now) {
    // Check if footswitch is held long enough to request reboot
    if (!reboot && controls.GetFootswitch(0).Held() > 3000) {
        reboot = true;

        // Trigger LEDs to blink once (one-shot)
        controls.GetLed(0).BlinkFor(500, 50, now);
        controls.GetLed(1).BlinkFor(500, 50, now);
    }

    if (reboot) {
        // Regular update keeps blinking alive
        controls.GetLed(0).Update(now);
        controls.GetLed(1).Update(now);

        // When LED[0] finishes blinking, reset
        if (!controls.GetLed(0).IsBlinking()) {
            reboot = false;
            System::ResetToBootloader();
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
    hw.SetAudioBlockSize(BLOCK_SIZE); // Number of samples handled per callback
    hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);

    // AUDIO
    hw.StartAudio(AudioCallback);
    hw.StartLog(false);

    // CONTROLS
    controls.Init(hw);

    // Effects
    effectManager.SetBlockSize(hw.AudioBlockSize());
    effectManager.AddEffect(&tuner);

    while (1)
    {   
        controls.Update();
        // logger.TestHardware();
        effectManager.ProcessControlGestures();
        effectManager.UpdateParameters();
        effectManager.UpdateUI();
        float effectRate = effectManager.GetActiveUpdateRateMs();

        // // REBOOT BOARD DEV ONLY !!!!!!!!
        rebootDfuUpdate(System::GetNow());

        System::Delay(effectRate);
    }
}
