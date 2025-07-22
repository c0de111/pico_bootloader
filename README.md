# RP2040 Dual-Slot Bootloader

This repository implements a simple dual-slot bootloader for the Raspberry Pi Pico W (RP2040), enabling "over-the-air" firmware updates via WIFI (https://github.com/c0de111/esign). It supports two independent firmware slots, and selects the latest valid version to boot. The firmware in each slot is identified by a header (first 256 Bytes), the standard "boot2" section is removed from the application firmware.  


## Project Structure

- `bootloader.c`: Main bootloader logic.
- `application1.c`: First firmware image.
- `application2.c`: Second firmware image.
- `application1_memmap.ld`, `application2_memmap.ld`, `bootloader_memmap.ld`: Custom linker scripts to place binaries at specific flash offsets.
- `firmware_header.h`: Definition of the shared firmware metadata header.

## ➕ Flash Memory Map

```
RP2040 Flash Memory Map (2 MB = 0x200000)
┌──────────────┬──────────────────────┬────────────┬──────────────────────────────────────┐
│   Address    │       Region         │   Size     │              Description             │
├──────────────┼──────────────────────┼────────────┼──────────────────────────────────────┤
│ 0x000000     │ Bootloader           │  64 KB     │ Custom bootloader                    │
│ 0x010000     │ Firmware Slot 0      │ 940 KB     │ FIRMWARE_FLASH_SIZE = 0xEB800        │
│ 0x0FB800     │ Firmware Slot 1      │ 940 KB     │ FIRMWARE_FLASH_SIZE = 0xEB800        │
│ 0x1E7000     │ Config & Reserved    │ 100 KB     │ Configuration, logos, OTA buffers    │
│ 0x200000     │ Flash End            │            │ End of 2 MB QSPI flash               │
└──────────────┴──────────────────────┴────────────┴──────────────────────────────────────┘

Firmware slots start at XIP_BASE + offset:
  - Slot 0: 0x10000000 + 0x010000 = 0x10010000
  - Slot 1: 0x10000000 + 0x0FB800 = 0x100FB800

Notes:
- Bootloader includes 256-byte boot2 at the start of the binary.
- Firmware binaries include a 256-byte firmware header at the start of the binary.
- "Config & Reserved" includes all flash-persistent settings and data.
```

## Firmware Header

Each firmware image embeds a metadata header of type `firmware_header_t` at the start of the binary:

```c
typedef struct __attribute__((packed)) {
    char magic[13];            // "inki_firmware"
    uint8_t valid_flag;        // 1 = valid, 0 = invalid
    char build_date[16];       // e.g., "250720"
    char git_version[32];      // e.g., "v1.0.7-1-abcdef"
    uint32_t firmware_size;    // Size in bytes
    uint8_t slot;              // 0 = Slot 0, 1 = Slot 1, 255 = Direct
    uint32_t crc32;            // Placeholder for future use
} firmware_header_t;
```

The header is defined individually in each firmware (`application1.c`, `application2.c`) and placed explicitly at the beginning of flash (first 256 Byte) by the linker. 

## Boot Decision Logic

The bootloader uses the following logic to select which firmware to boot:

1. **Validate Headers**: Check magic string and valid flag.
2. **Parse Git Versions**: Extract `major.minor.patch-build` from `git_version` string.
3. **Compare Versions**:
   - If both valid: Boot the newer one.
   - If one valid: Boot that one.
   - If none valid: Halt system.

Version parsing uses a custom structure:

```c
typedef struct {
    uint8_t major, minor, patch;
    uint32_t build;
} parsed_version_t;
```

Example decision output:

```
------ Looking for newest valid firmware------
[DEBUG] Parsed slot 0: 1.0.6 build 60 (ok=1)
[DEBUG] Parsed slot 1: 1.0.7 build 1 (ok=1)
→ Booting Slot 1 (newer)
```

## 🔧 Utilities and Scripts

A simple flashing script is provided:

```bash
#!/bin/bash

# Flash bootloader
openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg   -c "adapter speed 5000"   -c "program build/pico_bootloader.bin 0x10000000 reset exit"

# Flash application 1
openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg   -c "adapter speed 5000"   -c "program build/application1.bin 0x10008000 reset exit"

# Flash application 2
openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg   -c "adapter speed 5000"   -c "program build/application2.bin 0x100f3000 reset exit"
```

## Goals

- **Dual-Slot Updates**: Bootloader can choose from two firmware slots (Slot 0 and Slot 1).
- **OTA / WIFI / Updates**: Can switch slots after remote updates, update transfer and writing to flash can be completed and checked before new firmware is used.
