#include "daisy_seed.h"
#include "daisy_petal.h"
#include "daisysp.h"
#include "controls.h"
#include "globals.h"
#include "bpm.h"
#include "metronome.h"
#include "utils.h"

using namespace daisy;
using namespace daisysp;
using namespace daisy::seed;

DaisySeed hw;
extern CustomLooper looper;
BpmRunner bpmRunner;
MetronomeClick metronome;

static constexpr int MAX_LOOP_SECONDS = 60;   // Maximum loop duration
float DSY_SDRAM_BSS loopBuffer[MAX_BUFFER_SIZE];

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        // Input sample
        float inputSample = in[0][i];
        // Looper processing
        float loopedSample = looper.ProcessSample(inputSample);

        // Metronome clicks
        float metronomeClick = metronome.Process();
        out[0][i] = inputSample + loopedSample + metronomeClick;
    }
}

int main(void)
{
    SetupHardware();
    float bpm = 120.0f; // Set BPM
    float beatsPerMeasure = 4.0; // Define how many beats per loop
    bpmRunner.Init(bpm, beatsPerMeasure);
    metronome.Init(hw.AudioSampleRate());
    metronome.SetDuration(20.0f); // Click length: 20ms
    looper.Init(hw.AudioSampleRate(), bpm, beatsPerMeasure);
    hw.StartAudio(AudioCallback);
    hw.StartLog(false);
    while (1)
    {
        updateControls();  // Implement controls to start/stop recording & playback
        bpmRunner.Update();
        bpmRunner.SetBpm(linMap(knob6, 0.f, 1.f, 40.f, 280.f));
        blinkBpm(bpmRunner);
        if (bpmRunner.IsBeat())
        {                
            metronome.Trigger(bpmRunner.IsMeasure() ? ClickType::Accent : ClickType::Normal);
        }
        if (footswitch1State)
        {
            looper.StartRecording(footswitch1State);
        }
        else
        {
            looper.StopRecording(!footswitch1State);
        }
        if (footswitch2State)
        {
            looper.StartPlayback(footswitch2State);
        }
        else
        {
            looper.StopPlayback(!footswitch2State);
        }
        System::Delay(5);
    }
}
