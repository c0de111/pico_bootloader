#pragma once
#include <stdbool.h>

#define FIRMWARE_MAGIC "inki_firmware"
#define FIRMWARE_MAGIC_LEN (sizeof(FIRMWARE_MAGIC) - 1)
_Static_assert(FIRMWARE_MAGIC_LEN == 13, "FIRMWARE_MAGIC_LEN mismatch");

/*
 * RP2040 Flash Memory Map (2 MB = 0x200000)
 * ┌──────────────┬──────────────────────┬────────────┬──────────────────────────────────────┐
 * │   Address    │       Region         │   Size     │              Description             │
 * ├──────────────┼──────────────────────┼────────────┼──────────────────────────────────────┤
 * │ 0x000000     │ Bootloader           │  64 KB     │ Custom bootloader                    │
 * │ 0x010000     │ Firmware Slot 0      │ 940 KB     │ FIRMWARE_FLASH_SIZE = 0xEB800        │
 * │ 0x0FB800     │ Firmware Slot 1      │ 940 KB     │ FIRMWARE_FLASH_SIZE = 0xEB800        │
 * │ 0x1E7000     │ Config & Reserved    │ 100 KB     │ Configuration, logos, OTA buffers    │
 * │ 0x200000     │ Flash End            │            │ End of 2 MB QSPI flash               │
 * └──────────────┴──────────────────────┴────────────┴──────────────────────────────────────┘
 *
 * Firmware slots start at XIP_BASE + offset:
 *   - Slot 0: 0x10000000 + 0x010000 = 0x10010000
 *   - Slot 1: 0x10000000 + 0x0FB800 = 0x100FB800
 *
 * Notes:
 * - Bootloader include 256-byte boot2 at the start of the binary.
 * - Firmware binaries include a 256-byte firmware header at the start of the binary.
 * - "Config & Reserved" includes all flash-persistent settings and data.
 */

#define BOOTLOADER_FLASH_OFFSET           0x000000 // bootloader (size: 64 kb) starts with boot2 (256 byte), directly after that vectortable custom bootloader
#define FIRMWARE_SLOT0_FLASH_OFFSET       0x000000 + 0x010000  // = 0x010000 (size: 940 kb) has to be identical in application1_memmap.ld and flash.sh
#define FIRMWARE_SLOT1_FLASH_OFFSET       0x000000 + 0x010000 + 0xEB800 // = 0x0FB800 (size: 940 kb) has to be identical in application1_memmap.ld and flash.sh
#define CONFIG_FLASH_OFFSET               0x1E7000  // Start of reserved flash region (top 100 KB)
#define VECTOR_TABLE_OFFSET               0x100  // first 256 Byte of respective firmware store firmware_header_t, than vector_table; no boot2

#ifndef FLASH_PAGE_SIZE
#define FLASH_PAGE_SIZE                   256  // 0x100 = 256, entspricht (1u << 8)
#endif

#define FIRMWARE_FLASH_SIZE               0xEB800  // 962,560 bytes, 235 flash sectors, 4 kb each, 940 kb

// Flash access
#define FLASH_PTR(offset)          ((const uint8_t *)(XIP_BASE + (offset)))
// #define FLASH_SECTOR_SIZE          0x1000  // already defined in pico SDK

typedef struct __attribute__((packed)) {
    char magic[13];             // "inki_firmware"
    uint8_t valid_flag;         // 1 = gültig
    char build_date[16];        // z. B. "2025-07-16 13:30"
    char git_version[32];       // "v1.0.6-60-gcddef-dirty"
    uint32_t firmware_size;     // Byte-Anzahl der Firmwaredaten
    uint8_t slot;               // 0 = slot0, 1 = slot1, 255 = direct
    uint32_t crc32;             // CRC über Firmwaredaten
    uint8_t reserved[185];      // Reserve für spätere Erweiterung
} firmware_header_t;
