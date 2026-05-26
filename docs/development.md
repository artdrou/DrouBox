# Development Guide

## Prerequisites

- **OS**: Linux, macOS, or WSL2 on Windows
- **Toolchain**: `arm-none-eabi-gcc >= 10`
- **CMake**: >= 3.22
- **dfu-util**: for flashing over USB

Run `./tools/setup.sh` to install everything automatically (Ubuntu/Debian/macOS).

---

## Build System

The repo uses a single CMake tree with two build modes:

| Mode | Command | Output |
|---|---|---|
| Firmware (ARM) | `cmake -B build -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi.cmake` | `.elf` + `.bin` per effect |
| Host tests | `cmake -B build-tests -DBUILD_TESTS=ON` | Test executables |

### Firmware build

```bash
# Configure (do once)
cmake -B build -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi.cmake \
      -DLIBDAISY_DIR=../libDaisy \
      -DDAISYSP_DIR=../DaisySP

# Build all firmwares
cmake --build build -j$(nproc)

# Build a single target
cmake --build build --target firmware_delay
```

Generated binaries are in `build/`:
```
build/firmware_delay.bin
build/firmware_distortion.bin
build/firmware_looper.bin
build/firmware_tuner.bin
build/firmware_multieffect.bin
```

### Tests

```bash
cmake -B build-tests -DBUILD_TESTS=ON
cmake --build build-tests -j$(nproc)
ctest --test-dir build-tests -V
```

Tests cover DSP algorithms only (no Daisy SDK required). Run in CI without hardware.

---

## Flashing

### DFU (recommended)

1. Connect Daisy Seed via USB
2. Hold **BOOT** button, tap **RESET**, release **BOOT** — Daisy enters DFU mode
3. Run:

```bash
./tools/flash.sh <effect>
# e.g.:
./tools/flash.sh delay
./tools/flash.sh multieffect
```

### Using make (legacy projects only)

```bash
cd Delay && make program-dfu
```

---

## Project Structure

### Library (`lib/`)

```
lib/
├── include/
│   ├── controls/       ← hardware abstraction (KnobsManager, FootswitchManager, ...)
│   ├── core/           ← EffectBase, EffectManager
│   ├── dsp/            ← LFO, ShelfFilter, Yin, Compressor, DcBlock
│   ├── effects/        ← Gain, Tuner, Distortion, DelayEffect, LooperEffect
│   └── utils/          ← mapping functions
└── src/
    └── (mirrors include/)
```

The library compiles in two modes:
- **Firmware**: full library including controls (requires Daisy SDK)
- **Tests**: `droubox_dsp` — DSP + utils only, no Daisy dependency

### Firmware (`firmware/`)

Each subdirectory is a self-contained firmware:
- `common/` — shared hardware config (`controlConfig.h`, `effectConfig.h`)
- `<effect>/main.cpp` — hardware init, audio callback, main loop

All firmwares use the same `controlConfig.h` (same physical box).

---

## Adding a New Effect

### 1. Create the effect class

```cpp
// lib/include/effects/myeffect.h
#pragma once
#include "effectsBase.h"

class MyEffect : public EffectBase {
public:
    explicit MyEffect(Controls& c) : EffectBase(c) {}
    void UpdateParameters() override;
    void Process(const float* in, float* out, size_t size) override;
    void UpdateUI() override;
private:
    float param_ = 0.f;
};
```

```cpp
// lib/src/effects/myeffect.cpp
#include "myeffect.h"
#include "controls.h"

void MyEffect::UpdateParameters() {
    param_ = controls_.GetKnobs().GetValue(0);
}

void MyEffect::Process(const float* in, float* out, size_t size) {
    for (size_t i = 0; i < size; ++i)
        out[i] = in[i] * param_;
}

void MyEffect::UpdateUI() {
    controls_.GetLed(0).Set(effectActive);
}
```

### 2. Register in CMake

In `lib/CMakeLists.txt`, add `src/effects/myeffect.cpp` to both the firmware and DSP source lists as appropriate.

### 3. Add to multieffect firmware

In `firmware/multieffect/main.cpp`:

```cpp
#include "myeffect.h"
static MyEffect myEffect(controls);
// ...
manager.AddEffect(&myEffect);
```

### 4. Write tests

Add `tests/dsp/test_myeffect.cpp` and register it in `tests/CMakeLists.txt`.

---

## Coding Conventions

- **C++17**, no exceptions (`-fno-exceptions`), no RTTI (`-fno-rtti`)
- **No heap allocation** inside `Process()` (audio ISR)
- **Naming**: `PascalCase` classes, `camelCase` methods, `SCREAMING_SNAKE` constants
- **Headers**: `#pragma once` only, no `#include` of Daisy headers in DSP-only files
- **Namespaces**: DSP classes use `droubox::`, Daisy-dependent classes use global scope
- **Parameter smoothing**: `smooth = α * new + (1 - α) * old` — document α choice

---

## Debugging

The Daisy Seed exposes a USB CDC serial port when `hw.StartLog(false)` is called.

```cpp
hw.PrintLine("freq: %d Hz", (int)frequency);
```

Connect with any serial terminal at 115200 baud, or:

```bash
# Linux
screen /dev/ttyACM0 115200

# macOS
screen /dev/cu.usbmodem* 115200
```

To reset to DFU from firmware without physical button access:

```cpp
System::ResetToBootloader();
```

This is wired to holding FS1 for 3 seconds in all firmwares.

---

## Continuous Integration

Add `.github/workflows/test.yml` to run the host test suite on every push:

```yaml
name: Tests
on: [push, pull_request]
jobs:
  test:
    runs-on: ubuntu-latest
    steps:
      - uses: actions/checkout@v4
      - name: Configure
        run: cmake -B build-tests -DBUILD_TESTS=ON
      - name: Build
        run: cmake --build build-tests -j4
      - name: Test
        run: ctest --test-dir build-tests -V
```
