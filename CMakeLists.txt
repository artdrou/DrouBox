cmake_minimum_required(VERSION 3.22)

# ── Build-mode selection ───────────────────────────────────────────────────────
# Host-side tests (native compiler, no Daisy):
#   cmake -B build-tests -DBUILD_TESTS=ON
#   cmake --build build-tests && ctest --test-dir build-tests -V
#
# Firmware (ARM cross-compile):
#   cmake -B build -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi.cmake
#   cmake --build build
# ──────────────────────────────────────────────────────────────────────────────
option(BUILD_TESTS "Build host-side unit tests (no Daisy SDK required)" OFF)

if(BUILD_TESTS)
    project(droubox-tests LANGUAGES CXX)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    enable_testing()
    add_subdirectory(lib)
    add_subdirectory(tests)
else()
    # ARM firmware — toolchain file must be passed on the cmake command line
    set(LIBDAISY_DIR "${CMAKE_SOURCE_DIR}/../libDaisy" CACHE PATH "Path to libDaisy root")
    set(DAISYSP_DIR  "${CMAKE_SOURCE_DIR}/../DaisySP"  CACHE PATH "Path to DaisySP root")

    project(droubox LANGUAGES C CXX ASM)
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)

    add_subdirectory(${LIBDAISY_DIR} ${CMAKE_BINARY_DIR}/libDaisy EXCLUDE_FROM_ALL)
    add_subdirectory(${DAISYSP_DIR}  ${CMAKE_BINARY_DIR}/DaisySP  EXCLUDE_FROM_ALL)
    add_subdirectory(lib)
    add_subdirectory(firmware)
endif()
