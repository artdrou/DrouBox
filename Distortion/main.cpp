#include "disto.h"
#include "utils.h"
#include "controls.h"
#include "filters.h"
#include "compressor.h"

#include "daisy_seed.h"
#include "daisy_petal.h"
#include "daisysp.h"

// TODO: Implement only 1 distortion style, tube amp
// 1 - Preamp gain stages + soft clipping (asymmetrical) x2 + input high pass
// 2 - Tone Stack EQ, Biquad filters, bass, treble
// 3?- push pull style phase inversion and shift?
// 3 - 

using namespace daisy;
using namespace daisysp;
using namespace daisy::seed;

DaisySeed hw;
Compressor compressor;

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
	float amplifiedIn = 0.f;
    float compressedIn = 0.f;
	for (size_t i = 0; i < size; i++)
	{
        for (int k = 0; k <= 1; k++) // Itère sur 0 et 1, left and right
        {
            if (onOff)
            {
                if (switches[2].state == SwitchState::LEFT)
                {
                    compressedIn = compressor.processSample(in[k][i]);
                    amplifiedIn = processDistortionFromDipsValue(ReadDipSwitches(), compressedIn, knob1, knob3);
                    out[k][i] = ProcessFilters(amplifiedIn, k);
                }
                else if (switches[2].state == SwitchState::RIGHT)
                {
                    amplifiedIn = processDistortionFromDipsValue(ReadDipSwitches(), in[k][i], knob1, knob3);
                    compressedIn = compressor.processSample(amplifiedIn);
                    out[k][i] = ProcessFilters(compressedIn, k);
                }
                else
                {
                    amplifiedIn = processDistortionFromDipsValue(ReadDipSwitches(), in[k][i], knob1, knob3);
                    out[k][i] = ProcessFilters(amplifiedIn, k);
                }
            }  
            else
            {
                out[k][i] = in[k][i];
            }  
        }
	}
}

int main(void)
{
	SetupHardware();
    SetupFilters(hw.AudioSampleRate());
    compressor.Init(hw.AudioSampleRate());
	hw.StartAudio(AudioCallback);
    hw.StartLog(false); // Start USB CDC (serial over USB)
	while(1) {
        updateControls();
        compressor.UpdateCompressor(knob4, knob5, knob6);
        UpdateFilters(knob2, switches[0].state, switches[1].state);
        System::Delay(5);
    }
}
