#!/usr/bin/env bash
# DrouBox development environment setup
# Installs arm-none-eabi toolchain, libDaisy, DaisySP
# Supports Ubuntu/Debian and macOS

set -euo pipefail

REPO_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
PARENT_DIR="$(dirname "$REPO_DIR")"

echo "=== DrouBox Setup ==="
echo "Repo: $REPO_DIR"
echo "SDK dir: $PARENT_DIR"
echo ""

# ── 1. Toolchain ──────────────────────────────────────────────────────────────
install_toolchain() {
    if command -v arm-none-eabi-gcc &>/dev/null; then
        echo "[✓] arm-none-eabi-gcc already installed ($(arm-none-eabi-gcc --version | head -1))"
        return
    fi

    echo "[→] Installing arm-none-eabi toolchain..."
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        sudo apt-get update -q
        sudo apt-get install -y gcc-arm-none-eabi binutils-arm-none-eabi \
             cmake ninja-build dfu-util git
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        brew install --cask gcc-arm-embedded
        brew install cmake ninja dfu-util git
    else
        echo "ERROR: Unsupported OS '$OSTYPE'. Install arm-none-eabi-gcc manually."
        exit 1
    fi
    echo "[✓] Toolchain installed."
}

# ── 2. libDaisy ───────────────────────────────────────────────────────────────
install_libdaisy() {
    local dir="$PARENT_DIR/libDaisy"
    if [[ -d "$dir" ]]; then
        echo "[✓] libDaisy already present at $dir"
        (cd "$dir" && git pull --ff-only 2>/dev/null || true)
    else
        echo "[→] Cloning libDaisy..."
        git clone https://github.com/electro-smith/libDaisy.git "$dir"
    fi
    echo "[→] Building libDaisy..."
    (cd "$dir" && make -j"$(nproc 2>/dev/null || sysctl -n hw.ncpu)")
    echo "[✓] libDaisy ready."
}

# ── 3. DaisySP ────────────────────────────────────────────────────────────────
install_daisysp() {
    local dir="$PARENT_DIR/DaisySP"
    if [[ -d "$dir" ]]; then
        echo "[✓] DaisySP already present at $dir"
        (cd "$dir" && git pull --ff-only 2>/dev/null || true)
    else
        echo "[→] Cloning DaisySP..."
        git clone https://github.com/electro-smith/DaisySP.git "$dir"
    fi
    echo "[→] Building DaisySP..."
    (cd "$dir" && make -j"$(nproc 2>/dev/null || sysctl -n hw.ncpu)")
    echo "[✓] DaisySP ready."
}

install_toolchain
install_libdaisy
install_daisysp

echo ""
echo "=== Setup complete ==="
echo ""
echo "Next steps:"
echo "  cmake -B build -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi.cmake"
echo "  cmake --build build --target firmware_multieffect"
echo "  ./tools/flash.sh multieffect"
echo ""
echo "Run host tests (no hardware):"
echo "  cmake -B build-tests -DBUILD_TESTS=ON"
echo "  cmake --build build-tests && ctest --test-dir build-tests -V"
