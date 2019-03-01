#include <LEDControl.h>
#include <tlc_fades.h>

//======= CHANGED IN GUI ============
extern uint16_t micThreshold;
extern uint8_t musicSen;
extern uint8_t lightMode;
extern uint8_t brightness;
extern uint8_t currentPage;
extern uint8_t numSteps;
extern uint8_t scene[MAX_LIGHT_STEPS][NUM_MUGS];
extern uint16_t fadeDuration;    //Fading is default off
extern uint32_t stepDuration;		
//===Colour Picker Variables =======
extern uint8_t guiColourID[4];	
extern bool stepsUpDown;

uint32_t currentMillis = 0;    // stores the value of millis() in each iteration of loop()

bool DirectionChange = false;

//======= CONSTANT GLOBAL VARS ============
const uint16_t pwmTable[] PROGMEM = {
	0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4,
	4, 4, 5, 5, 5, 5, 5, 5, 6, 6, 6, 6, 6, 6, 7, 7, 7, 7, 8, 8, 8, 8, 9, 9, 9, 10, 10, 10, 11, 11, 11, 12, 12, 12, 13, 13, 14, 14,
	15, 15, 15, 16, 17, 17, 18, 18, 19, 19, 20, 21, 21, 22, 23, 24, 24, 25, 26, 27, 28, 29, 29, 30, 31, 32, 34, 35, 36, 37, 38, 39,
	41, 42, 43, 45, 46, 48, 49, 51, 53, 54, 56, 58, 60, 62, 64, 66, 68, 70, 73, 75, 78, 80, 83, 85, 88, 91, 94, 97, 100, 104, 107,
	111, 114, 118, 122, 126, 130, 134, 139, 143, 148, 153, 15, 163, 168, 174, 179, 185, 191, 198, 204, 211, 218, 225, 232, 240, 248,
	256, 264, 273, 282, 291, 301, 310, 321, 331, 342, 353, 365, 377, 389, 402, 415, 429, 443, 457, 472, 488, 504, 520, 537, 555, 573,
	592, 611, 631, 652, 673, 695, 718, 742, 766, 791, 817, 844, 871, 900, 929, 960, 991, 1024, 1057, 1092, 1128, 1165, 1203, 1242, 1283,
	1325, 1368, 1413, 1460, 1507, 1557, 1608, 1661, 1715, 1771, 1829, 1889, 1951, 2015, 2081, 2149, 2220, 2292, 2368, 2445, 2525, 2608,
	2693, 2782, 2873, 2967, 3064, 3165, 3268, 3375, 3486, 3600, 3718, 3840, 3966, 4045, 4095, 4095}; //Lookup Table for mapping 255 steps logarithmicly to the 4096 PWM

const uint8_t colourTable[10][3] = { //Colour Table => Each colour has an ID
 //G,R,B
  {0,0,0},       //Black		0
  {215,255,215}, //White   		1
  {255,0,0},     //Green   		2
  {0,255,0},     //Red     		3
  {0,0,255},     //Blue    		4
  {200,255,0},   //Yellow		5
  {180,0,255},   //Cyan    		6 
  {0,255,200},   //Magenta  	7
  {155,255,0},   //Orange   	8
  {204,102,245}, //Light blue	9
};

LEDControl::LEDControl() {
	//======= GENERAL VARS ============
	currentStep = 0; //Number of the current Step
	currentKrug = 0; //Current Krug, where colour is changed
	krugCounter = 0; //For updating uneaven muge channels
	direction = 1;
	//stepsUpDown = 0; //TODO: Change by GUI!!
	
	//======= FADE FUNC VARS ============
	previous_fadeLEDs_Millis = 0;   //Timers for the functions
	
	for(uint8_t k=0;k<3;k++){
		currentColour[k] = 0;  //Colours in longPWM values of the current step
		nextColour[k] = 0;  //Colours in longPWM values of the current step
	}
	currentColourID = 0;
	nextColourID = 0;
	
	//====== SOUND DETECTION ===========
	microphonePin = A0; // select the input pin for the potentiometer
	microphoneValue = 0; // variable to store the value coming from the sensor
}
	
void LEDControl::setupTLC() const {
	Tlc.init();
	Tlc.clear();
}

void LEDControl::fadeLEDs(){
	
	//Check value of microphone
	if(musicSen>0) {
		microphoneValue = analogRead(microphonePin);
	} else microphoneValue = 0;
		
	//Either timer or musicControl can enable the step switch
    //if((currentMillis - previous_fadeLEDs_Millis >= stepDuration+fadeDuration)) { 
	if((currentMillis - previous_fadeLEDs_Millis >= stepDuration+fadeDuration) || (microphoneValue > micThreshold) ) { 
		
		for(currentKrug=0; currentKrug<NUM_MUGS; currentKrug++) {
        
			if(!(currentKrug%2)) {  //IMPORTANT: Update the counter every second time, because all LEDs are shifted attached
			  krugCounter = (currentKrug/2)*4;
			}
			
			currentColourID = scene[currentStep][currentKrug]; //Get the colour ID of the current Krug
			if(DirectionChange==false && direction==1)	nextColourID = scene[(currentStep+1)%numSteps][currentKrug]; //Get the colour ID of the current Krug
			else if (DirectionChange==false && direction==0)	nextColourID = scene[(currentStep-1)%numSteps][currentKrug]; //Get the colour ID of the current Krug
			else if(DirectionChange==true && direction==1)	nextColourID = scene[(currentStep-1)%numSteps][currentKrug];
			else if(DirectionChange==true && direction==0)	nextColourID = scene[(currentStep+1)%numSteps][currentKrug];	
			
			updateKrugColour(currentColourID,nextColourID);
		}
		
		if(stepsUpDown) {
			updateCurrentStepUpDown();	//Updates depending on direction but then when direction is switching also the nextColourID must be updated
		}
		else {
			updateCurrentStep();
		}
		//Reset Timers
		if(microphoneValue > micThreshold)	previous_fadeLEDs_Millis = currentMillis;
		else previous_fadeLEDs_Millis = previous_fadeLEDs_Millis + stepDuration + fadeDuration;
    }
    
	//if(currentPage!=1) {  //If there is SPI communication with SD card dont do anythign with TLCs
		tlc_updateFades(currentMillis);	
    //}
    
}

void LEDControl::calculateCurrentColour(uint8_t colourID) { //Calculates the real PWM value (0-4096) based on brightness and colour ID
	uint8_t smallPwmLevel[3] = {colourTable[colourID][RED],colourTable[colourID][GREEN],colourTable[colourID][BLUE]};
	uint8_t maxRGB = max(max(smallPwmLevel[RED],smallPwmLevel[GREEN]),smallPwmLevel[BLUE]); //Get the minimum value of the colour
	int16_t pwmLevel[3];
	for(uint8_t k=0;k<3;k++){   //Calculate the small PWM level depending on the brightness
		if(smallPwmLevel[k]>0) { //Only if value is bigger than 0
			pwmLevel[k] = smallPwmLevel[k] - maxRGB + brightness; //Get small PWM value of red and adjust it to the brightness
			if(pwmLevel[k]<0)	pwmLevel[k] = 0;
		} else if(smallPwmLevel[k]==0) {
			pwmLevel[k] = 0;
		}
	}
	
	//Map the small PWM level logarithmicly to the real PWM level
	currentColour[RED] = pgm_read_word_near(pwmTable + pwmLevel[RED]); 
	currentColour[GREEN] = pgm_read_word_near(pwmTable + pwmLevel[GREEN]);
	currentColour[BLUE] = pgm_read_word_near(pwmTable + pwmLevel[BLUE]);
}

void LEDControl::calculateNextColour(uint8_t colourID) { //Calculates the real PWM value (0-4096) based on brightness and colour ID
	uint8_t smallPwmLevel[3] = {colourTable[colourID][RED],colourTable[colourID][GREEN],colourTable[colourID][BLUE]};
	uint8_t maxRGB = max(max(smallPwmLevel[RED],smallPwmLevel[GREEN]),smallPwmLevel[BLUE]); //Get the minimum value of the colour
	int16_t pwmLevel[3];
	for(uint8_t k=0;k<3;k++){   //Calculate the small PWM level depending on the brightness
		if(smallPwmLevel[k]>0) { //Only if value is bigger than 0
			pwmLevel[k] = smallPwmLevel[k] - maxRGB + brightness; //Get small PWM value of red and adjust it to the brightness
			if(pwmLevel[k]<0) pwmLevel[k] = 0;	
		} else if(smallPwmLevel[k]==0) {
			pwmLevel[k] = 0;
		}
	}
	nextColour[RED] = pgm_read_word_near(pwmTable + pwmLevel[RED]); //Map the small PWM level logarithmicly to the real PWM level
	nextColour[GREEN] = pgm_read_word_near(pwmTable + pwmLevel[GREEN]);
	nextColour[BLUE] = pgm_read_word_near(pwmTable + pwmLevel[BLUE]);
}

void LEDControl::updateKrugColour(uint8_t bufCurrentColourID, uint8_t bufNextColourID) {
	
	switch(bufCurrentColourID) {
		case 0:
			bufCurrentColourID = guiColourID[0];
			break;
		case 1:
			bufCurrentColourID = guiColourID[1];
			break;
		case 2:
			bufCurrentColourID = guiColourID[2];
			break;
		case 3:
			bufCurrentColourID = guiColourID[3];
			break;
	}
	
	switch(bufNextColourID) {
		case 0:
			bufNextColourID = guiColourID[0];
			break;
		case 1:
			bufNextColourID = guiColourID[1];
			break;
		case 2:
			bufNextColourID = guiColourID[2];
			break;
		case 3:
			bufNextColourID = guiColourID[3];
			break;
	}
	
	calculateCurrentColour(bufCurrentColourID);
    calculateNextColour(bufNextColourID);       
    //Set the Fade values for each 
	if (lightMode==1) {
		currentColour[RED] = Tlc.get(currentKrug+krugCounter);
		currentColour[GREEN] = Tlc.get(currentKrug+krugCounter+2);
		currentColour[BLUE] = Tlc.get(currentKrug+krugCounter+4);
	}
    tlc_addFade(currentKrug+krugCounter, currentColour[RED], nextColour[RED], currentMillis, currentMillis + fadeDuration);
    tlc_addFade(currentKrug+krugCounter+2, currentColour[GREEN], nextColour[GREEN], currentMillis, currentMillis + fadeDuration);
    tlc_addFade(currentKrug+krugCounter+4, currentColour[BLUE], nextColour[BLUE], currentMillis, currentMillis + fadeDuration);
}

void LEDControl::updateCurrentStep() {
	currentStep = (currentStep+1) % numSteps; //Update the current step. If last step is over, set it to first step
	//Serial.println(currentStep);
}

void LEDControl::updateCurrentStepUpDown() {
	//Serial.println(currentStep);
	//Switch direction if end is reached
	if(DirectionChange==true) {
		if(currentStep<=0) {
			direction = 1;	//Count up to numSteps
			currentStep = 0;
			//Serial.println(currentStep);
		}
		else if(currentStep>=numSteps-1) {	//SOLUTION: Hier numSteps-1 wahrscheinlich
			direction = 0;	//Count down to 0
			currentStep = numSteps-1;
			//Serial.println(currentStep);
		}	
		
		//Reset the change Flag
		DirectionChange = false;
	}
																			  
								  
						   
								
   
	//Serial.println(currentStep);
	
	//Update step depending on direction
	if(direction==1) {
		currentStep++;	//Step counting upwards
		if(currentStep==numSteps-1) DirectionChange = true;
	}
	else if(direction==0) {
		currentStep--;	//Steps going downwards
		if(currentStep==0) DirectionChange = true;
	}
	
	//Serial.println(currentStep);
}



//======================ANIMATIONS=======================================================================
//=======================================================================================================

void LEDControl::randomColours(){
	
	//Check value of microphone
	if(musicSen>0) {
		microphoneValue = analogRead(microphonePin);
	} else microphoneValue = 0;
	
	//Either timer or musicControl can enable the step switch
    if((currentMillis - previous_fadeLEDs_Millis >= stepDuration+fadeDuration) || (microphoneValue > micThreshold) ) { 
			
		for(currentKrug=0; currentKrug<NUM_MUGS; currentKrug++) {
        
			if(!(currentKrug%2)) {  //IMPORTANT: Update the counter every second time, because single RGB LEDs are wired with one channel gap (e.g. LED1= Ch. 1,3,5)
			  krugCounter = (currentKrug/2)*4;
			}
			currentColourID = nextColourID; //Get random colour ID;
			
			nextColourID = random(1,10); //Get random colour ID from 1 to 9 (Black is 0 and excluded)
			updateKrugColour(currentColourID,nextColourID);
		}
		
		if(stepsUpDown) {
			updateCurrentStepUpDown();	//Update currentStep depending on direction
		}
		else {
			updateCurrentStep();
		}
		//Reset the timer
		if(microphoneValue > micThreshold)	previous_fadeLEDs_Millis = currentMillis;
		else previous_fadeLEDs_Millis = previous_fadeLEDs_Millis + stepDuration + fadeDuration;
    }
    
	//if(currentPage!=1) {  //If there is SPI communication with SD card dont do anythign with TLCs
		tlc_updateFades(currentMillis);
    //}
    
}
