// controls.h
#ifndef CONTROLS_H
#define CONTROLS_H

#include "daisy_seed.h"
#include "daisy_petal.h"
#include "daisysp.h"

using namespace daisy;
using namespace daisysp;
using namespace daisy::seed;

// --- Global Hardware Objects ---
extern DaisySeed hw;

// Footswitches
extern Switch footswitch1, footswitch2;
extern bool footswitch1State, footswitch2State;

// DIP Switches
extern dsy_gpio dip1, dip2, dip3, dip4;
extern int dips_last_value;
extern int dips_current_value;

// Enum for switch state
enum class SwitchState
{
    LEFT,
    OFF,
    RIGHT
};

// onOffOnSwitch class
class onOffOnSwitch
{
  public:
    void Init(dsy_gpio_pin leftPin, dsy_gpio_pin rightPin);
    void Update();
    void UpdateLastState();
    const char* switchStateToString(SwitchState state);

    SwitchState GetState() const;
    SwitchState GetLastState() const;
    
    SwitchState state;
  private:
    dsy_gpio switchesPositions[2];
    SwitchState lastState = SwitchState::OFF;
};

// Declare your switches externally if needed
extern onOffOnSwitch switches[3];

// Control Parameters
extern float onOff;
extern float knob1;
extern float knob2;
extern float knob3;
extern float knob4;
extern float knob5;
extern float knob6;

// --- ADC Channels ---
enum AdcChannel
{
    KNOB_1,
    KNOB_2,
    KNOB_3,
    KNOB_4,
    KNOB_5,
    KNOB_6,
    NUM_ADC_CHANNELS
};

// --- Setup Functions ---
void SetupHardware();
void SetupFootswicthes();
void SetupDips();
void SetupSwitches();
void SetupKnobs();

// --- DIP & Switch Utilities ---
int ReadDipSwitches();
void GetDipSwitchesStates(int states[4]);
void GetSwitchesStates();

// --- Update Functions ---
void updateFootswitches();
void updateDips();
void updateSwitches();
void updateKnobs();
void updateControls();

// --- Utils functions ---
float smoothKnobValue(float currentValue, float lastSmoothedValue, float alpha);


#endif // CONTROLS_H
