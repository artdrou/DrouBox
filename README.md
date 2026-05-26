# DrouBox

Multi-effects guitar pedal firmware for the [Daisy Seed](https://electro-smith.com/daisy) microcontroller (STM32H750 @ 480 MHz).

Five independent firmware targets — each flashes as a standalone effect pedal — plus a shared C++ library covering all DSP, controls, and effect abstractions.

---

## Effects

| Firmware | Description | Knobs | Controls |
|---|---|---|---|
| `delay` | Stereo delay with LFO modulation and DC-blocked feedback | Time L/R, Feedback, Blend, Mod Rate, Depth | FS1 bypass, gesture navigation |
| `distortion` | 10 distortion types with compressor and EQ | Drive, Tone, Level, Sustain, Attack, Blend | DIP selects type, FS1 bypass |
| `looper` | 60-second loop recorder (SDRAM) | BPM | FS1 record toggle, FS2 playback toggle |
| `tuner` | Chromatic YIN pitch detector, guitar-string matching | — | LEDs show tuning direction |
| `multieffect` | All effects in one binary, gesture-switchable | All 6 knobs per active effect | FS-A tap = bypass, FS-A hold = prev, FS-B hold = next, both tap = lock, both hold = global bypass |

### LED tuner feedback
- Both LEDs bright: in tune (brightness = accuracy)
- Left LED only: pitch too low
- Right LED only: pitch too high

---

## Repository Layout

```
DrouBox/
├── CMakeLists.txt          ← top-level build (firmware or tests)
├── cmake/
│   └── arm-none-eabi.cmake ← ARM Cortex-M7 toolchain
├── lib/                    ← droubox shared library
│   ├── CMakeLists.txt
│   ├── include/            ← controls/, core/, dsp/, effects/, utils/
│   └── src/
├── firmware/
│   ├── common/             ← shared hardware config
│   ├── delay/
│   ├── distortion/
│   ├── looper/
│   ├── tuner/
│   └── multieffect/
├── tests/                  ← host Catch2 tests (no hardware needed)
├── docs/                   ← hardware pin map, development guide, per-effect docs
└── tools/
    ├── setup.sh            ← install toolchain + SDK
    └── flash.sh            ← flash firmware via DFU
```

---

## Quick Start

### 1. Install toolchain and SDKs

```bash
./tools/setup.sh
```

Installs `arm-none-eabi-gcc`, clones `libDaisy` and `DaisySP` as siblings of this repo, and builds them.

### 2. Build a firmware

```bash
cmake -B build -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi.cmake
cmake --build build --target firmware_delay
```

Build all firmwares at once:

```bash
cmake --build build
```

### 3. Flash to Daisy Seed

Put the Daisy into DFU mode: hold **BOOT**, tap **RESET**, then release **BOOT**.

```bash
./tools/flash.sh delay
```

Equivalent to:
```bash
dfu-util -a 0 -s 0x08000000:leave -D build/firmware_delay.bin
```

### 4. Run tests (no hardware needed)

```bash
cmake -B build-tests -DBUILD_TESTS=ON
cmake --build build-tests
ctest --test-dir build-tests -V
```

---

## Hardware

See [docs/hardware.md](docs/hardware.md) for full pin map, BOM, and wiring.

| Component | Daisy Pins |
|---|---|
| 6× Knobs | A1, A2, A3, A4, A5, A6 |
| 2× Footswitches | D25, D26 |
| 2× LEDs | A7, A8 |
| 3× On-Off-On switches | D14+D13 / D7+D10 / D2+D4 |
| 4× DIP switches | D1, D3, D5, D6 |

---

## Development

See [docs/development.md](docs/development.md) for the full workflow.

### Adding a new effect in 4 steps

1. `lib/include/effects/myeffect.h` — inherit `EffectBase`
2. `lib/src/effects/myeffect.cpp` — implement `UpdateParameters`, `Process`, `UpdateUI`
3. Add source to `lib/CMakeLists.txt`
4. Add to `firmware/multieffect/main.cpp`

---

## Audio Specs

| Parameter | Value |
|---|---|
| Sample rate | 48 kHz |
| Block size | 512 samples |
| Latency | ~10.7 ms |
| Format | 32-bit float |

---

## License

MIT
