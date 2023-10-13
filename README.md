## Getting Started

This project must be located in the <SDK>/projects/target_apps/template

It uses the HJ-531IMF module which contains 1 Mbit of SPI flash memory, 0x20000 in HEX.

Because of the flash memory, and use of FUOTA, it requires a custom bootloader that is included in the final image.

This sectors the flash memory into 3 sections: bootloader, application, and EEPROM data. Application is divided in half to allow for OTA updates.

[user_version.h](./src/user_version.h) is used to set the version number of the application. This is used by the bootloader to determine if an update is needed.

## Programming flash

1. Solder module to an adapter board with JTAG connection through JLINK cable or Dialog Pro Dev Kit (no shorting jumpers needed)
2. Build the project with Keil uVision, then copy `out_DA14531/Objects/handleit_sensor_531.bin` to `input` and rename to the laest version corresponding with `user_version.h`
3. Make single image from the `input/sensor-x-x-x.bin` in SmartSnippets Toolbox. This will be the application image.
4. Using SmartSnippets Toolbox (set to 0x20000 flash memory size), program the combined image to the device. This should be nearly 128 KB in size and contain the bootloader and 2 copies of the application. One of the application slots is just a placeholder.
5. Add the application image to cloud storage and update the latest sensor env var.

## Resources

[DA14531 main resources page](https://www.renesas.com/us/en/products/interface-connectivity/wireless-communications/bluetooth-low-energy/da14531-smartbond-ultra-low-power-bluetooth-51-system-chip)

[DA14531 TINY main resources page](https://www.renesas.com/us/en/products/interface-connectivity/wireless-communications/bluetooth-low-energy/da14531mod-smartbond-tiny-bluetooth-low-energy-module)

[SDK6 GitHub examples](https://github.com/dialog-semiconductor/BLE_SDK6_examples)

[SDK6 Tutorial](http://lpccs-docs.dialog-semiconductor.com/Tutorial_SDK6/introduction.html)

[SUOTA alternative tutorial](http://lpccs-docs.dialog-semiconductor.com/Tutorial-BLE-SUOTA-DA145x/flashing_and_upgrading.html)

[DA14531 TINY datasheet](https://www.renesas.com/us/en/document/dst/da14531-module-datasheet) - reference for pinout

[Dev Kit Pro user manual](https://www.renesas.com/kr/en/document/mat/um-b-114-da14531-development-kit-pro-hardware-user-manual) - hardware overview. Pin assignments on page 22

[DA14531 TINY Daughterboard user manual](https://www.renesas.com/us/en/document/mat/um-b-141-da14531-smartbond-tiny-module-development-kit-pro-hardware-user-manual?r=1580481) - daughterboard overview. UART configurations on page 20

[Battery life calcuator](https://www.digikey.com/en/resources/conversion-calculators/conversion-calculator-battery-life)