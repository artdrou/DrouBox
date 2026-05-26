# Hardware Reference

> **PCB design based on [GuitarML FunBox](https://github.com/GuitarML/FunBox).**
> Buffer architecture and PCB layout are credited to the GuitarML project.

## Daisy Seed Pin Map

| Function | Daisy Pin | Notes |
|---|---|---|
| Knob 1 (ADC) | A1 | 10kΩ pot, wiper to pin |
| Knob 2 (ADC) | A2 | |
| Knob 3 (ADC) | A3 | |
| Knob 4 (ADC) | A4 | |
| Knob 5 (ADC) | A5 | |
| Knob 6 (ADC) | A6 | |
| Footswitch 1 | D25 | Normally-open, to GND, active-low |
| Footswitch 2 | D26 | Normally-open, to GND, active-low |
| LED 1 | A7 | PWM, 220Ω series resistor to GND |
| LED 2 | A8 | PWM, 220Ω series resistor to GND |
| On-Off-On SW1 pin A | D14 | |
| On-Off-On SW1 pin B | D13 | |
| On-Off-On SW2 pin A | D7  | |
| On-Off-On SW2 pin B | D10 | |
| On-Off-On SW3 pin A | D2  | |
| On-Off-On SW3 pin B | D4  | |
| DIP SW1 | D1 | Active-low (internal pull-up) |
| DIP SW2 | D3 | |
| DIP SW3 | D5 | |
| DIP SW4 | D6 | |

## ADC Knob Wiring

```
3.3V ──┬── 10kΩ pot ──┬── GND
       │              │
       └── wiper ─────┴── Daisy ADC pin
```

## Footswitch Wiring

```
Daisy pin (D25/D26) ──── Footswitch ──── GND
(internal pull-up enabled in firmware)
```

## LED Wiring

```
Daisy pin (A7/A8) ──── 220Ω ──── LED (anode) ──── GND
```

## On-Off-On Switch Wiring

3-position toggle switch. Center pin connected to GND, two outer pins to Daisy GPIOs.

```
          ┌── Pin A (D14) ← LEFT position
GND ── center
          └── Pin B (D13) ← RIGHT position
              (floating = OFF)
```

## DIP Switch Wiring

Standard 4-position DIP switch, active-low:

```
Daisy GPIO ──── DIP pin ──── GND
(internal pull-up enabled)
```

## BOM (Bill of Materials)

| Component | Quantity | Specs |
|---|---|---|
| Daisy Seed | 1 | Electro-Smith |
| 10kΩ audio-taper potentiometer | 6 | Panel-mount |
| Footswitch | 2 | SPST momentary, normally-open |
| 3mm LED | 2 | Any color |
| 220Ω resistor | 2 | 1/4W |
| On-Off-On toggle switch | 3 | SPDT center-off |
| 4-position DIP switch | 1 | |
| 1/4" TRS jack | 2 | Input + Output |
| 9V DC barrel jack | 1 | Center-negative |
| Enclosure | 1 | 1590BB or similar |

## Power

The Daisy Seed requires 3.3V–6V. Use a 9V → 3.3V regulator or the Daisy's built-in regulator fed by a 9V supply.

```
9V supply ── regulator ── Daisy DGND + VIN
```

## Audio I/O

The Daisy Seed has a built-in WM8731 codec:
- Input: 3.5mm stereo jack or line-level
- Output: 3.5mm stereo jack or line-level
- For a guitar pedal, use a 1/4" instrument-level jack with an input impedance buffer (1MΩ input impedance recommended)
