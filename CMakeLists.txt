# Minimum required version of CMake
cmake_minimum_required(VERSION 3.12)

set(PICO_NO_TOOLCHAIN_POSTPROCESS 1)
set(PICO_BOARD pico_w)
set(PICO_SDK_PATH "/home/nicolas/pico/pico-sdk")

# Include the Pico SDK and set up the project
include(${PICO_SDK_PATH}/pico_sdk_init.cmake)

project(pico_bootloader C CXX ASM)

# Set C/C++ standards
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_executable(pico_bootloader
    bootloader.c          # Main application logic
)

add_executable(application1
    application1.c          # Main application logic
)
add_executable(application2
    application2.c          # Main application logic
)

pico_set_linker_script(application1 ${CMAKE_CURRENT_SOURCE_DIR}/application1_memmap.ld)
pico_set_linker_script(application2 ${CMAKE_CURRENT_SOURCE_DIR}/application2_memmap.ld)
pico_set_linker_script(pico_bootloader ${CMAKE_CURRENT_SOURCE_DIR}/bootloader_memmap.ld)

option(USB_BOOTLOADER_ENABLE "Enable USB support in bootloader" OFF)

if(USB_BOOTLOADER_ENABLE)
    pico_enable_stdio_usb(pico_bootloader 1)
    add_definitions(-DUSB_BOOTLOADER_ENABLE=1)
else()
    pico_enable_stdio_usb(pico_bootloader 0)
endif()

# Enable USB output, disable UART output
pico_enable_stdio_usb(application1 1)
pico_enable_stdio_usb(application2 1)
pico_enable_stdio_uart(application1 0)
pico_enable_stdio_uart(application2 0)

# Add support for extra output formats (map/bin/hex/uf2)
pico_add_extra_outputs(application1)
pico_add_extra_outputs(application2)

# Link the required libraries to the project
target_link_libraries(pico_bootloader
    pico_stdlib                    # Standard Pico SDK library
)
target_link_libraries(application1
    pico_stdlib                    # Standard Pico SDK library
)
target_link_libraries(application2
    pico_stdlib                    # Standard Pico SDK library
)
