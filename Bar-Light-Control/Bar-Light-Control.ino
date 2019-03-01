/*
Project: Bar-Light-Control
Author:  Linus Reitmayr

Description:  Arduino Project for controlling LEDs build into my bar using TLC5940 drivers and Touchscreen Interface
              Main code in this project is in the written libraries which itself depend on 3rd-Party Libraries

Depenences: 
            Third-Party Libs:
            TLC5940
            UTFT
            GLS1680
            sdFat
            
            Own Libs:
            Filebrowser
            ledTFT
            LEDControl       

 */

#include <LEDControl.h>

LED_TFT displayControl;
LEDControl ledController;
extern uint32_t currentMillis;  
extern uint8_t lightMode;       

void setup()
{
  /*  Only for DEBUG
    Serial.begin(9600); //Only for debug
    delay(100);
    Serial.println("Start Bar Control");
  */

  //Setup the Hardware
  randomSeed(analogRead(6));  //Used for randomColour generation
  ledController.setupTLC();
  displayControl.setupLCD();
  
  
}

void loop() {
  //Handle all display settings/controls
  displayControl.updateDisplay();
  
  // capture the latest value of millis() -> Used for non-blocking delay
  currentMillis = millis();
   
  //For all Mugs one light step is made with or without fading
  switch(lightMode) { //Choose which operation mode schould be used
     case 0:
        ledController.fadeLEDs(); //Mode 0 is normal Light steps without fading
        break;
     case 1:
        ledController.randomColours(); //Mode 1 is random colours
        break;
      default:
        ledController.fadeLEDs(); //Default is normal Light steps without fading
  }
}
