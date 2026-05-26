#!/usr/bin/env bash
# Flash a DrouBox firmware target to Daisy Seed via DFU
#
# Usage:  ./tools/flash.sh <target> [build_dir]
#   target    : delay | distortion | looper | tuner | multieffect
#   build_dir : path to cmake build directory (default: build)
#
# Before running: put Daisy in DFU mode
#   Hold BOOT → tap RESET → release BOOT

set -euo pipefail

EFFECT="${1:-}"
BUILD_DIR="${2:-build}"

if [[ -z "$EFFECT" ]]; then
    echo "Usage: $0 <effect> [build_dir]"
    echo "  Effects: delay | distortion | looper | tuner | multieffect"
    exit 1
fi

BIN="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)/$BUILD_DIR/firmware_${EFFECT}.bin"

if [[ ! -f "$BIN" ]]; then
    echo "ERROR: $BIN not found."
    echo "Build first: cmake --build $BUILD_DIR --target firmware_${EFFECT}"
    exit 1
fi

if ! command -v dfu-util &>/dev/null; then
    echo "ERROR: dfu-util not found. Install it:"
    echo "  Ubuntu/Debian: sudo apt install dfu-util"
    echo "  macOS:         brew install dfu-util"
    exit 1
fi

echo "Flashing $EFFECT firmware..."
echo "  Binary : $BIN"
echo "  Target : STM32H750 at 0x08000000"
echo ""
echo "Make sure the Daisy Seed is in DFU mode (hold BOOT, tap RESET)."
echo ""

dfu-util -a 0 -s 0x08000000:leave -D "$BIN"

echo ""
echo "[✓] Flash complete. The Daisy Seed will now boot the $EFFECT firmware."
