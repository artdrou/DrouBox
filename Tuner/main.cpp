#include "daisy_seed.h"
#include "daisy_petal.h"
#include "daisysp.h"
#include "controls.h"
#include <utils/mapping.h>

using namespace daisy;
using namespace daisysp;
using namespace daisy::seed;

DaisySeed hw;

static constexpr int MAX_LOOP_SECONDS = 60;   // Maximum loop duration
static constexpr int MAX_BUFFER_SIZE = MAX_LOOP_SECONDS*48000;   // Maximum loop duration
float DSY_SDRAM_BSS loopBuffer[MAX_BUFFER_SIZE];

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        out[0][i] = in[0][i];
    }
}

int main(void)
{
    SetupHardware();
    hw.StartAudio(AudioCallback);
    hw.StartLog(false);
    while (1)
    {
        updateControls();
        System::Delay(5);
    }
}
