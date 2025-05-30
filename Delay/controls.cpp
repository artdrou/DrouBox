#include "controls.h"
#include "daisy_seed.h"
#include "daisy_petal.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;
using namespace daisy::seed;

// Hardware instance
extern DaisySeed hw;

// LED instances
Led led1, led2;

// Footswitch instances
Switch footswitch1, footswitch2;

onOffOnSwitch switches[3];

// DIP switch pins
dsy_gpio dip1, dip2, dip3, dip4;
int dips_last_value = -1; // Invalid initial value to force first print
int dips_current_value = -1;

// Control variables
float onOff;
float knob1;
float knob2;
float knob3;
float knob4;
float knob5;
float knob6;

float lastValueKnob1;
float lastValueKnob2;
float lastValueKnob3;
float lastValueKnob4;
float lastValueKnob5;
float lastValueKnob6;

// Blinking leds variables
bool blink_active = false;
int blink_count = 0;
uint32_t last_blink_time = 0;
bool justStartedBlink = false;
const int blink_total = 10;
const int blink_interval_ms = 50;
bool bothLedState = false;

// Setup Functions
void SetupFootswitches() {
    footswitch1.Init(D25, 1000);
    footswitch2.Init(D26, 1000);
}

void SetupLeds() {
    led1.Init(A7, true, 1000);
    led2.Init(A8, true, 1000);
}

void SetupDips() {
    dsy_gpio_pin dipsPins[4] = {
        hw.GetPin(1),
        hw.GetPin(3),
        hw.GetPin(5),
        hw.GetPin(6)
    };

    dsy_gpio* dips[4] = { &dip1, &dip2, &dip3, &dip4 };

    // Initialize DIP switches
    for (int i = 0; i < 4; i++) {
        dips[i]->pin  = dipsPins[i];
        dips[i]->mode = DSY_GPIO_MODE_INPUT;
        dips[i]->pull = DSY_GPIO_PULLUP;
        dsy_gpio_init(dips[i]);
    }
}

// Initialize the GPIO pins
void onOffOnSwitch::Init(dsy_gpio_pin leftPin, dsy_gpio_pin rightPin)
{
    switchesPositions[0].pin  = leftPin;
    switchesPositions[0].mode = DSY_GPIO_MODE_INPUT;
    switchesPositions[0].pull = DSY_GPIO_PULLUP;
    switchesPositions[1].pin  = rightPin;
    switchesPositions[1].mode = DSY_GPIO_MODE_INPUT;
    switchesPositions[1].pull = DSY_GPIO_PULLUP;
    dsy_gpio_init(&switchesPositions[0]);
    dsy_gpio_init(&switchesPositions[1]);
}

// Update the state
void onOffOnSwitch::Update()
{
    bool leftState  = !dsy_gpio_read(&switchesPositions[0]);
    bool rightState = !dsy_gpio_read(&switchesPositions[1]);
    if (leftState && !rightState)
        state = SwitchState::LEFT;
    else if (!leftState && rightState)
        state = SwitchState::RIGHT;
    else
        state = SwitchState::OFF;
}

void onOffOnSwitch::UpdateLastState()
{
    lastState = state;
}

const char* onOffOnSwitch::switchStateToString(SwitchState state) {
    switch (state) {
        case SwitchState::LEFT: return "LEFT";
        case SwitchState::OFF: return "OFF";
        case SwitchState::RIGHT: return "RIGHT";
        default: return "UNKNOWN";
    }
}

// Get current state
SwitchState onOffOnSwitch::GetState() const
{
    return state;
}

// Get current state
SwitchState onOffOnSwitch::GetLastState() const
{
    return lastState;
}

void SetupSwitches() {
    switches[0].Init(hw.GetPin(14), hw.GetPin(13));
    switches[1].Init(hw.GetPin(7), hw.GetPin(10));
    switches[2].Init(hw.GetPin(2),  hw.GetPin(4));
}

void SetupKnobs() {
    // ADC channels for knobs
    AdcChannelConfig adc_config[NUM_ADC_CHANNELS];
    
    // Initialize ADC channels for knobs
    adc_config[KNOB_1].InitSingle(A1);
    adc_config[KNOB_2].InitSingle(A2);
    adc_config[KNOB_3].InitSingle(A3);
    adc_config[KNOB_4].InitSingle(A4);
    adc_config[KNOB_5].InitSingle(A5);
    adc_config[KNOB_6].InitSingle(A6);
    
    // Initialize the ADC peripheral
    hw.adc.Init(adc_config, NUM_ADC_CHANNELS);
    hw.adc.Start(); // Start ADC conversion
}

// Hardware Initialization
void SetupHardware() {
    hw.Configure();
    hw.Init();
    hw.SetAudioBlockSize(512); // Number of samples handled per callback
    hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);

    SetupFootswitches();
    SetupLeds();
    SetupDips();
    SetupSwitches();
    SetupKnobs();

    // Turn on LEDs to indicate initialization
    hw.SetLed(true);
    led1.Set(true);
    led2.Set(true);
}

// LED Control Functions
void updateLeds() {
    led1.Update();
    led2.Update();
}

void blinkBothLeds() {
    if (blink_active) {
        uint32_t now = System::GetNow();
        
        // Initialize blink start time ONCE
        if (justStartedBlink) {
            last_blink_time = now;
            justStartedBlink = false;  // Only set once!
        }

        // Blink LED toggle every interval
        if ((now - last_blink_time) >= blink_interval_ms) {
            bothLedState = !bothLedState; // Toggle LED state
            led1.Set(!bothLedState);
            led2.Set(!bothLedState);
            updateLeds();
            last_blink_time = now;  // Update AFTER toggle

            // Count blinks and stop after a certain number of blinks
            if (!bothLedState) { // Only count when turning off
                blink_count++;
                if (blink_count >= blink_total) {
                    blink_active = false;
                    // Ensure LEDs stay off after last blink
                    led1.Set(!onOff);
                    led2.Set(true);
                    blink_count = 0;
                    last_blink_time = 0;
                    bothLedState = false;
                    updateLeds();
                }
            }
        }
    }
}

// Footswitch Update
void updateFootswitches() {
    footswitch1.Debounce();  // Update switch state
    footswitch2.Debounce();  // Update switch state
    
    if (footswitch1.RisingEdge()) {
        onOff = !onOff;
        hw.PrintLine("Footswitch 1 toggled");
        led1.Set(!onOff);
    }
    
    if (footswitch2.RisingEdge()) {
        hw.PrintLine("Footswitch 2 toggled");
    }
    
    // Enter DFU mode if Footswitch 1 is held for more than 3 seconds
    if (footswitch1.TimeHeldMs() > 3000) {
        hw.PrintLine("Entering DFU Mode...");
        System::ResetToBootloader(); // Restart in bootloader mode
    }

    // Start blinking if both footswitches are held for more than 1 second
    if (!blink_active && footswitch1.Pressed() && footswitch2.Pressed() &&
        footswitch1.TimeHeldMs() > 1000 && footswitch2.TimeHeldMs() > 1000) {
        hw.PrintLine("Both footswitches held for 1 second");
        blink_active = true;
        justStartedBlink = true;  // Will trigger initial blink
        blink_count = 0;
        bothLedState = false;
    }

    // Handle blinking LEDs
    if (blink_active) {
        blinkBothLeds();
    }
}

int ReadDipSwitches()
{
    int value = 0;
    value |= (!dsy_gpio_read(&dip1)) << 3; // DIP1 → Bit 3
    value |= (!dsy_gpio_read(&dip2)) << 2; // DIP2 → Bit 2
    value |= (!dsy_gpio_read(&dip3)) << 1; // DIP3 → Bit 1
    value |= (!dsy_gpio_read(&dip4)) << 0; // DIP4 → Bit 0
    return value;
}

void GetDipSwitchesStates(int states[4])
{
    states[0] = !dsy_gpio_read(&dip1);
    states[1] = !dsy_gpio_read(&dip2);
    states[2] = !dsy_gpio_read(&dip3);
    states[3] = !dsy_gpio_read(&dip4);
}

// DIP Switch Update
void updateDips() {
    int dips_current_value = ReadDipSwitches();
    if (dips_current_value != dips_last_value) {
        hw.PrintLine("DIP Switch State Changed: %d", dips_current_value);
        dips_last_value = dips_current_value;
    }
}

// Switch Update
void updateSwitches() {
    SwitchState state[3];
    SwitchState lastState[3];
    // Check if any switches' states have changed
    for (int j = 0; j < 3; j++)
    {
        switches[j].Update();
        lastState[j] = switches[j].GetLastState();
        
        state[j] = switches[j].GetState();
        if (state[j] != lastState[j]) {
            hw.PrintLine("Switch %d state changed: %s", j+1, switches[j].switchStateToString(state[j]));
            switches[j].UpdateLastState();
        }
    }
}

void updateKnobs()
{
    // Read ADC values (normalized to 0-1)
    lastValueKnob1 = knob1;
    lastValueKnob2 = knob2;
    lastValueKnob3 = knob3;
    lastValueKnob4 = knob4;
    lastValueKnob5 = knob5;
    lastValueKnob6 = knob6;
    // Read ADC values (normalized to 0-1)
    float smoothing = 0.5f;
    knob1 = smoothKnobValue(hw.adc.GetFloat(KNOB_1), lastValueKnob1, smoothing);
    knob2 = smoothKnobValue(hw.adc.GetFloat(KNOB_2), lastValueKnob2, smoothing);
    knob3 = smoothKnobValue(hw.adc.GetFloat(KNOB_3), lastValueKnob3, smoothing);
    knob4 = smoothKnobValue(hw.adc.GetFloat(KNOB_4), lastValueKnob4, smoothing);
    knob5 = smoothKnobValue(hw.adc.GetFloat(KNOB_5), lastValueKnob5, smoothing);
    knob6 = smoothKnobValue(hw.adc.GetFloat(KNOB_6), lastValueKnob6, smoothing);
}

// General Controls Update
void updateControls() {
    updateFootswitches();
    updateLeds();
    updateDips();
    updateSwitches();
    updateKnobs();
}

float smoothKnobValue(float currentValue, float lastSmoothedValue, float alpha)
{
    // Apply low-pass filter smoothing
    lastSmoothedValue = alpha * currentValue + (1.0f - alpha) * lastSmoothedValue;
    return lastSmoothedValue;
}
