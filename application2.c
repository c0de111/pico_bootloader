#include <stdio.h>
#include "pico/stdlib.h"
#include <string.h>  // for memcmp
#include "hardware/structs/scb.h"  // Für Zugriff auf SCB->VTOR
#include "hardware/sync.h"
#include "flash.h"

#define GATE_PIN 22 //GATE PIN, MOSFET for power supply control

void hold_power(void) {
    gpio_init(GATE_PIN);
    gpio_set_dir(GATE_PIN, GPIO_OUT);
    gpio_put(GATE_PIN, 1); // Drive the gate pin high to keep power on
    printf("Gate Pin on -> Power switch on\n");
}

void switch_off(void) {
    printf("Gate Pin off -> Power switch off\n");
    gpio_put(GATE_PIN, 0); // Drive the gate pin high to keep power on
}

__attribute__((section(".firmware_header")))
const firmware_header_t firmware_header = {
    .magic = FIRMWARE_MAGIC,
    .valid_flag = 0,
    .build_date = "250720",
    .git_version = "v1.1.30-11-dswddf",
    .firmware_size = 23252,
    .slot = 1,
    .crc32 = 0          // not implemented
};

_Static_assert(sizeof(firmware_header_t) == 256, "Firmware header must be exactly 256 bytes - check size of header! (arm-none-eabi-size -A application1.elf | grep firmware_header)");

void main(void) {

    // hold_power();

    sleep_ms(10);           // Allow debug interface to initialize
    stdio_init_all();       // Enable UART output for debug_log

    // Wait until USB-CDC is ready
    while (!stdio_usb_connected()) {
        sleep_ms(10);
    }

    printf("\n\n=== Hello Application 2 !===\n");

        while (1) {
            sleep_ms(500);  // Idle loop, could blink LED or enter USB recovery
            // printf("\n\n=== looping...===\n");
            // stdio_flush();
            // sleep_ms(50);
            // switch_off();
        }
}


