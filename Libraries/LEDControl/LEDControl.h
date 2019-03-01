/*  Touchscreen control for the bar light 
 *  Program made by Linus Reitmayr,
 *  
 */


#ifndef LEDControl_H
#define	LEDControl_H

#include <Tlc5940.h>
//#include <avr/pgmspace.h>

#include <ledTFT.h>

#define RED		0
#define GREEN		1
#define BLUE		2

class LEDControl {
	public:		
		//======= CONSTRUCTOR ============
		LEDControl();	//Constrcutor
		
		//======= PUBLIC FUNCTIONS ============
		void setupTLC() const;

		////======= Animations ============
		void fadeLEDs();
		void randomColours();
	
	private:

		
		//======= GENERAL VARS ============
		uint8_t currentStep; //Number of the current Step
		uint8_t currentKrug; //Current Krug, where colour is changed
		uint8_t krugCounter;
		bool direction;
		//bool stepsUpDown;
		
		//======= FADE FUNC VARS ============
		unsigned long previous_fadeLEDs_Millis;   //Timers for the functions
		uint16_t currentColour[3];  //Colours in longPWM values of the current step
		uint16_t nextColour[3];  //Colours in longPWM values of the current step
		uint8_t currentColourID;
		uint8_t nextColourID;
		
		//====== SOUND DETECTION ===========
		uint8_t  microphonePin; // select the input pin for the potentiometer
		uint16_t microphoneValue; // variable to store the value coming from the sensor
	
		//======= PRIVATE FUNCTIONS ============
		void calculateCurrentColour(uint8_t colourID);
		void calculateNextColour(uint8_t colourID);
		
		void updateKrugColour(uint8_t bufCurrentColourID, uint8_t bufNextColourID);																	 
		void updateCurrentStep();
		void updateCurrentStepUpDown();
};


#endif
