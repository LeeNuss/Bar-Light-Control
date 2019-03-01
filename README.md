# Bar-Light-Control

Arduino Project for controlling LEDs build into my bar using TLC5940 drivers and Touchscreen Interface.
Project is based on an ATmega2560 MCU
Display is a [5" TFT with 480x272 pixels](https://www.buydisplay.com/default/5-tft-ssd1963-lcd-module-touch-screen-display-480x272-mcu-arduino) using a SSD1963 controller and a capacitive touch panel with a [GLS1680 controller](https://linux-sunxi.org/GSL1680)


## Getting Started

### Hardware Setup

This project need a special hardware setup to work:
1. A display, compatible with the UTFT library --> If a different display resolution is used the ledTFT-Library must be modified to fit the screen size
2. If a resistive touch panel is used (recommended! The GLS1680 touch controller makes lot of problems) you can add the [URTouch library](http://www.rinkydinkelectronics.com/library.php?id=92). Again the ledTFT-Library (updateDisplay Function) has to be modified to read the coordinates.
3. A SD-Card the Folder [Light-Scenes](Light-Scenes) must be connected to the Arduino. Change the SPI-ChipSelect PIN in [ledTFT.h](Libraries/ledTFT/ledTFT.h)
4. The TLC5940 controllers must be connected to the Arduino according to the setup found on the [library GH-Page](https://github.com/PaulStoffregen/Tlc5940).

### Software Setup

These instructions 


### Used Libraries:

Used Third-party Libraries:
- Touch driver GSL1680:
https://github.com/ESTBLC/GSL1680

- Filesystem for SD card:
https://github.com/greiman/SdFat

- TLC5950 LED Driver:
https://github.com/PaulStoffregen/Tlc5940

- TFT Control:
http://www.rinkydinkelectronics.com/library.php?id=51

### Installing

A step by step series of examples that tell you how to get a development env running

```
Give the example
```


## Authors

* **Linus Reitmayr**

## License

This project is licensed under the MIT License - see the [LICENSE.md](LICENSE.md) file for details
