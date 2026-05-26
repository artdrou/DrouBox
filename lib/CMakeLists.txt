cmake_minimum_required(VERSION 3.22)

# ── droubox library ────────────────────────────────────────────────────────────
# When BUILD_TESTS=ON: DSP-only target (no Daisy, no controls)
# When building firmware: full target including hardware abstractions
# ──────────────────────────────────────────────────────────────────────────────

if(BUILD_TESTS)
    # Host-testable DSP subset — zero Daisy dependencies
    add_library(droubox_dsp STATIC
        src/dsp/lfo.cpp
        src/dsp/shelf.cpp
        src/dsp/yin.cpp
        src/dsp/compressor.cpp
        src/utils/mapping.cpp
    )
    target_include_directories(droubox_dsp
        PUBLIC
            include
            include/dsp
            include/utils
            include/controls
            include/core
            include/effects
    )
    target_compile_features(droubox_dsp PUBLIC cxx_std_17)
    add_library(droubox::dsp ALIAS droubox_dsp)

    # Controls logic with Daisy stubbed out for host testing
    add_library(droubox_controls STATIC
        src/controls/footSwitches.cpp
        src/controls/footSwitchPair.cpp
    )
    target_include_directories(droubox_controls
        PUBLIC
            include
            include/controls
        PRIVATE
            ${CMAKE_SOURCE_DIR}/tests/stubs
    )
    target_compile_features(droubox_controls PUBLIC cxx_std_17)
    add_library(droubox::controls ALIAS droubox_controls)
else()
    # Full firmware library — needs Daisy targets from parent scope
    add_library(droubox STATIC
        # Controls
        src/controls/controls.cpp
        src/controls/knobs.cpp
        src/controls/footSwitches.cpp
        src/controls/footSwitchPair.cpp
        src/controls/leds.cpp
        src/controls/dipSwitches.cpp
        src/controls/onOffOnSwitches.cpp
        # Core
        src/core/effectsManager.cpp
        # DSP
        src/dsp/lfo.cpp
        src/dsp/shelf.cpp
        src/dsp/yin.cpp
        src/dsp/fft.cpp
        src/dsp/compressor.cpp
        # Effects
        src/effects/gain.cpp
        src/effects/tuner.cpp
        src/effects/distortion.cpp
        src/effects/delay_effect.cpp
        src/effects/buffer.cpp
        src/effects/looper_effect.cpp
        # Utils
        src/utils/mapping.cpp
    )
    target_include_directories(droubox
        PUBLIC
            include
            include/dsp
            include/utils
            include/controls
            include/core
            include/effects
    )
    target_link_libraries(droubox PUBLIC daisy DaisySP)
    target_compile_features(droubox PUBLIC cxx_std_17)
    add_library(droubox::droubox ALIAS droubox)
endif()
