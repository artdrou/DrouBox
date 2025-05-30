#include "daisy_seed.h"
#include "daisy_petal.h"
#include "daisysp.h"
#include "controls.h"
#include "delay.h"  // Include the delay header

using namespace daisy;
using namespace daisysp;
using namespace daisy::seed;

DaisySeed hw;
Delay stereoDelay;  // Create a delay instance

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        for (int k = 0; k <= 1; k++) // Left (k=0) and Right (k=1)
        {
            if (onOff)
            {
                // Process delay
                out[k][i] = stereoDelay.processSample(in[k][i], k == 0);  // Pass true for left, false for right
            }
            else
            {
                // Process delay
                out[k][i] = in[k][i];
            }
        }
    }
}

int main(void)
{
    SetupHardware();
    stereoDelay.init(hw.AudioSampleRate());

    hw.StartAudio(AudioCallback);  // Start audio processing
    hw.StartLog(false);  // Start USB CDC (serial over USB)

    while (1)
    {
        updateControls();

        // You can also modify delay parameters dynamically here
        stereoDelay.setParameters(knob1, knob2, knob2, knob3, knob4, knob5);

        System::Delay(5);
    }
}
