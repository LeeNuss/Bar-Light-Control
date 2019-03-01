# Bar-Light-Control

**Arduino Project for controlling LEDs build into my bar using TLC5940 drivers and Touchscreen Interface**
* The Bar has 41 beer mugs with each containing 6 RGB-LEDs (controlled together). Therefore the colour of each mug can be controlled individually.
* Light-scenes are stored on a SD-Card and are simple textfiles that can be written on a computer.
* A big part of the project is a filebrowser API that is use by the GUI library to graphically choose a Light-scene from the SD-Card.
* In the Main Window the Lights can be controlled, like Fading, Music Control, Brightness and Speed.

## Important Info
Project is based on an ATmega2560 MCU (Arduino Mega) and will not run easily on any smaller MCU (Maybe with Resistive Touch screen be cause the touch control driver needs lots of memory). 
At this stage the project is very specific for my hardware setup therefore won't run easily on different setup. However some parts can be adapted for different projects (e.g. the GUI file explorer).


## Getting Started

### Hardware Setup

This project need a special hardware setup to work:
1. The display used is a [5" TFT with 480x272 pixels](https://www.buydisplay.com/default/5-tft-ssd1963-lcd-module-touch-screen-display-480x272-mcu-arduino) using a SSD1963 controller.
Generally any display compatible with the UTFT library can be used --> If a different display resolution is used the [ledTFT-Library](Libraries/ledTFT/) must be modified to fit the screen size
2. A Capacitive touch panel with a [GLS1680 controller](https://linux-sunxi.org/GSL1680).
If a resistive touch panel is used (recommended! The GLS1680 touch controller makes lot of problems) you can add e.g. the [URTouch library](http://www.rinkydinkelectronics.com/library.php?id=92). Again the [ledTFT-Library](Libraries/ledTFT/).
3. A SD-Card containing the Folder [Light-Scenes](Light-Scenes) is connected to the Arduino via SPI.
4. 8 TLC5940 controllers daisy-chained connected to the Arduino according to the setup found on the [library GH-Page](https://github.com/PaulStoffregen/Tlc5940).

### Software Setup

To make this project work the software has to be adapted to the used hardware:
1. Using a different screen resolution: Most of the [ledTFT-Library](Libraries/ledTFT/) must be modified to fit the screen size
2. Using a different touch panel the updateDisplay Function must be modified in [ledTFT.c](Libraries/ledTFT/ledTFT.c) to read the coordinates.
3. Add the Folder [Light-Scenes](Light-Scenes) to a SD-Card (Formatted in FAT or FAT32 system). Change the SPI-ChipSelect PIN in [ledTFT.h](Libraries/ledTFT/ledTFT.h) for your used CS-Pin
4. Modify the number of used TLC controllers in the [tlc_config.h](Libraries/Tlc5940/tlc_config.h). To use a different LED-setup the code in the [LEDControl-Library](Libraries/LEDControl) msut be hugly modified.


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
