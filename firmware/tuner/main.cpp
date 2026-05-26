#include "daisy_seed.h"
#include "daisysp.h"
#include "controls.h"
#include "effectsManager.h"
#include "tuner.h"
#include "controlConfig.h"

using namespace daisy;
using namespace daisy::seed;

static DaisySeed     hw;
static Controls      controls;
static EffectManager manager(controls);
static Tuner         tuner(controls, 2048);

static bool rebootPending = false;

static void HandleDfuReboot(uint32_t now) {
    if (!rebootPending && controls.GetFootswitch(0).Held() > 3000) {
        rebootPending = true;
        controls.GetLed(0).BlinkFor(500, 50, now);
        controls.GetLed(1).BlinkFor(500, 50, now);
    }
    if (rebootPending && !controls.GetLed(0).IsBlinking()) {
        System::ResetToBootloader();
    }
}

static void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size) {
    manager.Process(in[0], out[0], size);
    for (size_t i = 0; i < size; ++i) out[1][i] = out[0][i];
}

int main() {
    hw.Init(true);
    hw.SetAudioBlockSize(512);
    hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
    hw.StartAudio(AudioCallback);
    hw.StartLog(false);

    controls.Init(hw);

    manager.SetBlockSize(hw.AudioBlockSize());
    manager.AddEffect(&tuner);

    while (true) {
        uint32_t now = System::GetNow();
        controls.Update();
        manager.ProcessControlGestures();
        manager.UpdateParameters();
        manager.UpdateUI();
        HandleDfuReboot(now);
        System::Delay(static_cast<uint32_t>(manager.GetActiveUpdateRateMs()));
    }
}
