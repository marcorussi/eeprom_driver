# EEPROM_demo

A driver for 24C128/256 EEPROM develop on top of libopencm3 library for STM32F4 devices. The driver includes an example application tested with a STM32F4 Discovery board and external EEPROM over I2C bus.

For compiling the project it is necessary to download the libopencm3 library https://github.com/libopencm3/libopencm3. This project folder must be located in the same folder where libopencm3 is located. For compiling the project run:

    $ make

and for flashing the STM32F4Discovery board run:

    $ make flash

The default toolchain is the same of libopencm3, an arm-none-eabi/arm-elf toolchain.

