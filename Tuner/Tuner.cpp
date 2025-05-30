#include "daisy_seed.h"
#include "daisysp.h"
#include <math.h>
#include <complex>
#include <vector>

using namespace daisy;
using namespace daisysp;

DaisySeed hw;
float sample_rate;


Led led1, led2;

constexpr int FFT_SIZE = 1024; // Size of FFT, must be a power of 2
std::vector<std::complex<float>> fft_input(FFT_SIZE);

// Tuning Frequencies (Standard Guitar EADGBE)
constexpr float tuning_frequencies[] = {82.41, 110.00, 146.83, 196.00, 246.94, 329.63};

// Frequency Detection Variables
float detected_frequency = 0.0f;

float systemDelay = 0.1f;
float systemLedCount = 0.f;
bool led_state = false;


void FFT(std::vector<std::complex<float>>& data, bool invert = false)
{
    int n = data.size();
    int logN = log2(n);

    // Bit-reversal permutation
    for (int i = 0; i < n; ++i) {
        int rev = 0;
        for (int j = 0; j < logN; ++j) {
            if (i & (1 << j)) {
                rev |= 1 << (logN - 1 - j);
            }
        }
        if (rev > i) {
            std::swap(data[i], data[rev]);
        }
    }

    // Cooley-Tukey FFT
    for (int len = 2; len <= n; len *= 2) {
        float angle = 2.0 * M_PI / len * (invert ? -1 : 1);
        std::complex<float> wlen(cos(angle), sin(angle));
        
        for (int i = 0; i < n; i += len) {
            std::complex<float> w(1);
            for (int j = 0; j < len / 2; ++j) {
                std::complex<float> u = data[i + j];
                std::complex<float> v = data[i + j + len / 2] * w;
                data[i + j] = u + v;
                data[i + j + len / 2] = u - v;
                w *= wlen;
            }
        }
    }

    // Normalize if inverse FFT
    if (invert) {
        for (auto& x : data) {
            x /= n;
        }
    }
}


// Function to find the dominant frequency in the FFT result
float GetDominantFrequency(std::vector<std::complex<float>>& fft_data)
{
    float max_magnitude = 0;
    int max_index = 0;

    // Find peak in FFT result
    for (int i = 1; i < fft_data.size() / 2; i++) // Only look at positive frequencies
    {
        float magnitude = std::abs(fft_data[i]);
        if (magnitude > max_magnitude) {
            max_magnitude = magnitude;
            max_index = i;
        }
    }

    // Calculate the frequency corresponding to the peak bin
    float sample_rate = hw.AudioSampleRate(); // Get the sample rate
    return (float)max_index * sample_rate / FFT_SIZE;
}

// LED Control Based on Frequency
void UpdateLEDs()
{
    // float min_diff = 9999.0f;
    // float closest_freq = 0.0f;

    // // Find closest target frequency
    // for (float target : tuning_frequencies)
    // {
    //     float diff = fabsf(detected_frequency - target);
    //     if (diff < min_diff)
    //     {
    //         min_diff = diff;
    //         closest_freq = target;
    //     }
    // }

    // // LED Brightness Control
    // float brightness = 1.0f - (min_diff / 10.0f); // Normalize (close = brighter)
    // brightness = fmaxf(0.0f, fminf(brightness, 1.0f)); // Clamp 0 to 1

    // if (min_diff < 2.0f)
    // {
    //     led1.Set(1.0f);
    //     led2.Set(1.0f);
    // }
    // else if (detected_frequency < closest_freq)
    // {
    //     led1.Set(0.0f);
    //     led2.Set(brightness);
    // }
    // else
    // {
    //     led1.Set(brightness);
    //     led2.Set(0.f);
    // }
    // Set LEDs to full brightness for testing
    // led1.Set(true);
    // led2.Set(true);
    // led1.Update();
    // led2.Update();
}

// Initialize amplitude to calculate
float current_amplitude = 0.0f;
float target_brightness = 0.0f;  // Target brightness value (0.0 to 1.0)
float brightness = 0.0f;     // Current brightness value
// Smoothing factor (the higher the value, the slower the change)
float smooth_factor = 0.01f; // Tweak this value for more or less smoothness


// Audio Callback Function
void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
    for (size_t i = 0; i < size; ++i)
    {
        // Calculate amplitude by averaging the absolute values of the samples
        float amplitude_sum = 0.0f;
        
        for (size_t i = 0; i < size; i++)
        {
            amplitude_sum += fabsf(in[0][i]);  // Only need one channel for mono input
        }

        // Calculate average amplitude
        current_amplitude = 10.*amplitude_sum / size;

        // Normalize amplitude to the range of 0.0 to 1.0
        current_amplitude = fminf(current_amplitude, 1.0f); // Clamp to 1.0 to avoid overflow
        // Set the target brightness based on the calculated amplitude
        target_brightness = current_amplitude;
        out[0][i] = in[0][i];
        out[1][i] = in[1][i];
    }
}

void Blink() {
    if (systemLedCount > 500) {
        led_state = !led_state;
        systemLedCount = 0;
        hw.SetLed(led_state);
    }
    else {
        systemLedCount += 1;
    }
}

void UpdateLEDsBasedOnAmplitude()
{
    // Map amplitude (0.0 - 1.0) directly to LED brightness (0.0 - 1.0)
    // Smoothly adjust the current brightness towards the target brightness
    if (brightness < target_brightness) {
        smooth_factor = 0.5f;
    }
    else {
        smooth_factor = 0.001f;
    }
    brightness += smooth_factor * (target_brightness - brightness);
    brightness = fmaxf(brightness, 0.001f);


    // Set LED brightness based on amplitude
    led1.Set(brightness);  // Adjust LED1 brightness
    led1.Update();
    led2.Set(brightness);  // Adjust LED2 brightness
    led2.Update();
}

int main(void)
{
	hw.Init();

	sample_rate = hw.AudioSampleRate();
    
    // Initialize LED Pins
    led1.Init(daisy::seed::A7, false);
    led2.Init(daisy::seed::A8, false);

	hw.SetAudioBlockSize(4); // number of samples handled per callback
	hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
	hw.StartAudio(AudioCallback);

    // Blink();
	while(1) {
        // Continuously update LEDs based on the current input amplitude
        UpdateLEDsBasedOnAmplitude();
        Blink();
        System::Delay(systemDelay);  // Short delay for smooth LED update
    }
}
