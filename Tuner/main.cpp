#include "daisy_seed.h"
#include "daisy_petal.h"
#include "daisysp.h"
#include "leds.h"
#include "controls.h"
#include "footSwitchs.h"
#include <utils/mapping.h>

using namespace daisy;
using namespace daisysp;
using namespace daisy::seed;

DaisySeed hw;

// Global declaration because stack overflowed when declared in main
FootswitchManager fsw1, fsw2;
LedManager ledMgr1, ledMgr2;

// DFU reboot (DEV ONLY)
bool reboot = false;


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
        System::Delay(10);
    }
}
