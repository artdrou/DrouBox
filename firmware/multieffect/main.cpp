#include "daisy_seed.h"
#include "daisysp.h"
#include "controls.h"
#include "effectsManager.h"
#include "gain.h"
#include "tuner.h"
#include "distortion.h"
#include "delay_effect.h"
#include "controlConfig.h"
#include "effectConfig.h"

using namespace daisy;
using namespace daisy::seed;

// ── SDRAM for delay buffer ─────────────────────────────────────────────────
static constexpr size_t DELAY_BUFFER_SAMPLES = static_cast<size_t>(48000 * 0.5f);
float DSY_SDRAM_BSS delayBuffer[DELAY_BUFFER_SAMPLES];

static DaisySeed     hw;
static Controls      controls;
static EffectManager manager(controls);

static Gain                  gain(controls);
static Tuner                 tuner(controls, 2048);
static droubox::Distortion   disto(controls);
static droubox::DelayEffect  delay(controls);

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

    disto.Init(hw.AudioSampleRate());
    delay.Init(hw.AudioSampleRate());

    manager.SetBlockSize(hw.AudioBlockSize());
    // Effect order: Gain → Distortion → Delay → Tuner (tuner is transparent)
    manager.AddEffect(&gain);
    manager.AddEffect(&disto);
    manager.AddEffect(&delay);
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
