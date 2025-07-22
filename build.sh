#!/bin/bash

# Enable or disable USB support in the bootloader (1 = enabled, 0 = disabled)
USB_BOOTLOADER_ENABLE=1

# Print USB bootloader status in color
if [ "$USB_BOOTLOADER_ENABLE" -eq 1 ]; then
    echo -e "\033[1;31m[INFO] USB-UART Bootloader ENABLED -> careful, only for debugging! Jumping to the applications might not work.\033[0m"  # red
else
    echo -e "\033[1;32m[INFO] USB-UART Bootloader DISABLED\033[0m" # green
fi

# Define the build directory relative to the script's location
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="$SCRIPT_DIR/build"

# Create the build directory if it doesn't exist
if [ ! -d "$BUILD_DIR" ]; then
    mkdir -p "$BUILD_DIR"
fi

# Navigate to the build directory
cd "$BUILD_DIR" || exit 1

# Run CMake with USB option passed as a variable
cmake -DUSB_BOOTLOADER_ENABLE=${USB_BOOTLOADER_ENABLE} "$SCRIPT_DIR"

# Build all targets
make -j4

# Generate binary files from ELF files
arm-none-eabi-objcopy -O binary pico_bootloader.elf pico_bootloader.bin
arm-none-eabi-objcopy -O binary application1.elf application1.bin


