# DrouBox — CLAUDE.md

## Project Summary

DrouBox is a multi-effects guitar pedal firmware platform targeting the **Daisy Seed** microcontroller (STM32H750 @ 480 MHz). The project is written in C++17 and uses the **libDaisy** SDK and **DaisySP** audio library. Audio runs at 48 kHz / 32-bit float in real-time ISR callbacks.

## Repository Layout

```
DrouBox/
├── CLAUDE.md
├── README.md
├── CMakeLists.txt            ← top-level CMake (firmware or tests)
├── cmake/
│   └── arm-none-eabi.cmake   ← ARM Cortex-M7 toolchain file
├── .github/workflows/
│   └── tests.yml             ← CI: host DSP tests on push
├── lib/                      ← shared library (all reusable code)
│   ├── CMakeLists.txt
│   ├── include/
│   │   ├── constants.h
│   │   ├── controls/         ← knobs, footswitches, LEDs, dip, 3-pos
│   │   ├── core/             ← EffectBase, EffectManager
│   │   ├── dsp/              ← LFO, YIN, FFT, ShelfFilter, Compressor, DcBlock
│   │   ├── effects/          ← Gain, Tuner, Distortion, DelayEffect, LooperEffect
│   │   └── utils/            ← mapping functions
│   └── src/
├── firmware/
│   ├── CMakeLists.txt        ← add_firmware() macro + subdirectories
│   ├── common/
│   │   ├── controlConfig.h   ← hardware pin definitions (shared by all firmwares)
│   │   └── effectConfig.h    ← knob-to-parameter index structs
│   ├── delay/main.cpp
│   ├── distortion/main.cpp
│   ├── looper/main.cpp
│   ├── tuner/main.cpp
│   └── multieffect/main.cpp  ← all 4 effects in one binary
├── tests/                    ← Catch2 host-side tests (no hardware)
│   ├── CMakeLists.txt
│   └── dsp/                  ← test_lfo, test_shelf, test_yin, test_compressor,
│                                 test_distortion, test_mapping
├── docs/
│   ├── hardware.md           ← pin map, BOM, wiring
│   ├── development.md        ← dev setup, build, flash, add new effect
│   └── effects/              ← per-effect parameter reference
└── tools/
    ├── setup.sh              ← install toolchain + clone+build libDaisy/DaisySP
    └── flash.sh              ← dfu-util wrapper: ./tools/flash.sh <effect>
```

## Build Commands

```bash
# Firmware (ARM cross-compile)
cmake -B build -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi.cmake
cmake --build build --target firmware_multieffect

# Flash
./tools/flash.sh multieffect

# Host tests (no hardware)
cmake -B build-tests -DBUILD_TESTS=ON
cmake --build build-tests && ctest --test-dir build-tests -V
```

## Core Abstractions

### EffectBase (lib/include/core/effectsBase.h)
Abstract base for all effects. Implement:
- `UpdateParameters()` — read controls, update internal params
- `Process(in, out, size)` — audio callback (ISR, no heap alloc)
- `UpdateUI()` — update LEDs after processing

State: `effectActive` (bypass), `controlsActive` (whether this effect owns controls).

### EffectManager (lib/include/core/effectsManager.h)
- Holds list of `EffectBase*`
- Ping-pong buffer chain processing
- Footswitch gesture dispatch: ATap=bypass, AHeld=prev, BHeld=next, BothTapped=lock, BothHeld=global bypass
- Effect switching with LED feedback

### Controls Hierarchy

| Class | File | Purpose |
|---|---|---|
| `Controls` | controls/controls.h | Master: init + update all peripheral managers |
| `KnobsManager` | controls/knobs.h | 6 ADC pots, exponential smoothing (α=0.5) |
| `FootswitchManager` | controls/footSwitches.h | Debounce, tap/hold detection |
| `FootswitchPair` | controls/footSwitchPair.h | Combined gestures with lockout |
| `LedManager` | controls/leds.h | PWM + one-shot blink |
| `DipManager` | controls/dipSwitches.h | 4-position DIP bank, binary value |
| `OnOffOnSwitchManager` | controls/onOffOnSwitches.h | 3-position switch (LEFT/OFF/RIGHT) |

### DSP Primitives (all in `drouAudioLib::` or `droubox::`)

| Class | Namespace | File |
|---|---|---|
| `Lfo` | `drouAudioLib` | dsp/lfo.h |
| `ShelfFilter` | `drouAudioLib` | dsp/shelf.h |
| `Yin` | global | dsp/yin.h |
| `Compressor` | `droubox` | dsp/compressor.h |
| `DcBlock` | `droubox` | dsp/dc_block.h |

### Effects

| Class | Namespace | Notes |
|---|---|---|
| `Gain` | global | Simple gain, knob 0 |
| `Tuner` | global | YIN pitch detection, 2048-sample buffer |
| `Distortion` | `droubox` | 10 algorithms, compressor, EQ |
| `DelayEffect` | `droubox` | Stereo delay, LFO mod, DC-blocked feedback |
| `LooperEffect` | `droubox` | SDRAM-backed 60s loop |

## Hardware Pin Map (from firmware/common/controlConfig.h)

| Peripheral | Pins |
|---|---|
| Knobs (ADC) | A1–A6 |
| Footswitch 1 | D25 |
| Footswitch 2 | D26 |
| LED 1 | A7 |
| LED 2 | A8 |
| On-Off-On SW 1 | D14, D13 |
| On-Off-On SW 2 | D7, D10 |
| On-Off-On SW 3 | D2, D4 |
| DIP switches | D1, D3, D5, D6 |

## Audio Parameters

- Sample rate: 48 kHz
- Block size: 512 samples (~10.7 ms latency)
- Format: 32-bit float

## Coding Conventions

- C++17, no exceptions, no RTTI (embedded constraints)
- No dynamic allocation in `Process()` (audio ISR)
- `PascalCase` classes, `camelCase` methods, `SCREAMING_SNAKE` constants
- DSP-only code (no Daisy deps): `droubox::` namespace
- `#pragma once` in all headers
- Parameter smoothing: `smooth = α * new + (1-α) * old` — comment α value

## External Dependencies (siblings of this repo)

```
../libDaisy/    Daisy Seed HAL (Electro-Smith)
../DaisySP/     Audio DSP primitives (Electro-Smith)
```
