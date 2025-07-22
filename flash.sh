#!/bin/bash

set -e  # Exit on error

echo
echo "[INFO] Flashing pico_bootloader.bin to 0x10000000 ..."
openocd --debug=0 -f interface/cmsis-dap.cfg -f target/rp2040.cfg \
  -c "adapter speed 5000" \
  -c "program build/pico_bootloader.bin 0x10000000 reset exit"

sleep 0.5

echo
echo "[INFO] Flashing application1.bin to slot 0 at 0x10010000 ..."
openocd --debug=0 -f interface/cmsis-dap.cfg -f target/rp2040.cfg \
  -c "adapter speed 5000" \
  -c "program build/application1.bin 0x10010000 reset exit"

sleep 0.5

echo
echo "[INFO] Flashing application2.bin to slot 1 at 0x100FB800 ..."
openocd --debug=0 -f interface/cmsis-dap.cfg -f target/rp2040.cfg \
  -c "adapter speed 5000" \
  -c "program build/application2.bin 0x100FB800 reset exit"

echo
echo "[DONE] Flashing complete."
