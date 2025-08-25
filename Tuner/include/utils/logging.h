#pragma once
#include "controls.h"

#include "daisy_seed.h"

#include "logging.h"


using namespace daisy;

#include <cstddef>  // for size_t


class HardwareLogger {
public:
    HardwareLogger(Controls& controlsRef, DaisySeed& hwRef);

    // Call this repeatedly in main loop
    void TestHardware();

private:
    Controls& controls;
    DaisySeed& hw;
};
