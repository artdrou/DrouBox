#include "daisy_seed.h"
#include "daisysp.h"
#include "controls.h"
#include "effectsManager.h"
#include "looper_effect.h"
#include "controlConfig.h"
#include "effectConfig.h"

using namespace daisy;
using namespace daisy::seed;

static constexpr size_t MAX_LOOP_SECONDS = 60;
static constexpr size_t MAX_SAMPLES      = MAX_LOOP_SECONDS * 48000;

// SDRAM placement — survives past the stack
float DSY_SDRAM_BSS loopBuffer[MAX_SAMPLES];

static DaisySeed     hw;
static Controls      controls;
static EffectManager manager(controls);

static droubox::LooperEffect looper(controls);

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
    looper.Init(loopBuffer, MAX_SAMPLES, hw.AudioSampleRate());

    manager.SetBlockSize(hw.AudioBlockSize());
    manager.AddEffect(&looper);

    bool fs1Prev = false, fs2Prev = false;

    while (true) {
        uint32_t now = System::GetNow();
        controls.Update();
        manager.UpdateParameters();
        manager.UpdateUI();
        HandleDfuReboot(now);

        // Footswitch 1: record toggle
        bool fs1 = controls.GetFootswitch(0).GetState();
        if (fs1 && !fs1Prev) {
            if (looper.IsRecording()) looper.StopRecording();
            else                      looper.StartRecording();
        }
        fs1Prev = fs1;

        // Footswitch 2: playback toggle
        bool fs2 = controls.GetFootswitch(1).GetState();
        if (fs2 && !fs2Prev) {
            if (looper.IsPlaying()) looper.StopPlayback();
            else                    looper.StartPlayback();
        }
        fs2Prev = fs2;

        System::Delay(5);
    }
}
