/*  Touchscreen control for the bar light 
 *  Program made by Linus Reitmayr,
 *  
 */


#ifndef ledTFT_H
#define ledTFT_H

#include <UTFT.h>
#include <filebrowser.h>
#include <GSL1680.h>

#define FILELIST_Y_TOP 70
#define FILELIST_LINE_SIZE 30
#define MAX_LIST_ITEMS 6

#define SCREEN_RES_X	480
#define SCREEN_RES_Y	272

// SD card chip select pin.
#define SD_CS	47

//Touchscreen Pins
#define WAKE 41
#define INTRPT 48

class LED_TFT {
	public:
		//====== CONSTRUCTOR ==========
		LED_TFT();
		
		//======= PUBLIC FUNCTIONS ============
		void setupLCD();
		void updateDisplay();
		
	private:		
		//======= Touch position ============
		uint32_t x, y;

		//======= GUI VARS ============
		uint16_t xFadeSlid;   //Initial value for slider
		uint16_t xBrightSlid; //Initial value for slider
		uint16_t xStepSlid;
		uint16_t fadeDurationBuf;
		bool fadeSwitch;   //Fader set to off
		
		//========= Colour Picker Variables =========					
		uint8_t changedColourID;
		uint16_t getScreenColour(uint8_t colour);
		
		//======= Touchscreen functions =========
		void handleTouchLedSettings();
		void handleTouchFileBrowser();
		void handleTouchColourPicker();
		void handleTouchHelpMenu();

		//======Draw Screen Pages ===========================
		void drawLightControl();
		void drawFileExplorer();
		void drawColourPicker();
		void drawHelpMenu();
		
		//===================================================
		//Choose setting from SD-card
		void drawFileList();
		void fileClick(uint8_t index);
		void drawCurrentPath();	

		//====================================================
		//============= setLedColor() - Custom Funtion
		void sliderControl();
};
#endif