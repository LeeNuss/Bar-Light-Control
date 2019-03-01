/*  Touchscreen control for the bar light 
 *  Program made by Linus Reitmayr,
 *  
 */
 
#include "ledTFT.h"
//Define the colours to be used
#define FONT_COLOUR 		VGA_WHITE
#define BASE_COLOUR 		VGA_BLUE
#define BASE_COLOUR_2 		VGA_GRAY
#define HEADER_COLOUR 		VGA_GREEN
#define SLIDER_COLOUR 		VGA_YELLOW
#define SLIDEROFF_COLOUR 	VGA_GRAY
#define BACKGROUND_COLOUR 	VGA_BLACK

//Positioners
#define SLIDER_X		10
#define SLIDER1_Y 		150
#define SLIDER2_Y 		200
#define SLIDER3_Y 		250
#define SLIDER_HEIGHT	20
#define SLIDER_LENGTH	300
#define SLIDER_FONT_Y	20


//======= FONTS ============
extern uint8_t SmallFont[];
extern uint8_t BigFont[];
extern uint8_t Arial_round[];
extern uint8_t arial_bold[];
extern uint8_t Symbols_32[];
extern uint8_t arial_normal[];
extern uint8_t fg_normal[];
		
//========= Colour Picker Variables =========
uint8_t guiColourID[4] = {0,1,2,3};	//Colours which will be changed by GUI

//==== Creating Objects
UTFT myLCD(SSD1963_480,38,39,40,41);
FileBrowser browser(SD_CS);
GSL1680 myTouch = GSL1680();

//====== SOUND DETECTION =======
uint16_t micThreshold = 9999;
uint8_t musicSen = 0;

//Changed in help menu
uint16_t MIC_THRES_LOW	= 290;
uint16_t MIC_THRES_MID	= 240;
uint16_t MIC_THRES_HIGH	= 200;


//====== File Browser =======
uint8_t pageIndex = 0;
char currentSetting[STRING_LENGTH] = {};
uint8_t numFiles = MAX_LIST_ITEMS;
uint8_t numSteps = 0; //Number of the total Steps (Depends on the scene)
uint8_t scene[MAX_LIGHT_STEPS][NUM_MUGS] = {};  //Array storing the light scene

//====== LED CONTROL =======
uint8_t currentPage = 0;
uint8_t lightMode = 0;
uint8_t brightness = 255; //Brightness is percent
uint32_t stepDuration = 500;	//1 sec default
uint16_t fadeDuration = 0;    //Fading is default off
bool stepsUpDown = 0;

//Touchscreen
int xb=0;
int yb=0;

LED_TFT::LED_TFT() {

	//======= Touch position ============
	x = 0;
	y = 0;

	//======= GUI VARS ============
	xFadeSlid = 10;   //Initial value for slider
	xBrightSlid = 300; //Initial value for slider
	xStepSlid = 150; //Initial value for slider
	fadeDurationBuf = 0;    //Fading is default off
	fadeSwitch = 0;   //Fader set to off
	changedColourID = 0; //Colour Which will be changed

}

//====== Draw the Main Screen Pages ==============
//Control light settings
void LED_TFT::drawLightControl() {
	myLCD.clrScr();
	//Set right page:
	currentPage = 0;
	
	//========== HEADER ====================
	//========================================
	myLCD.setColor(HEADER_COLOUR);
	myLCD.fillRect(0,0,SCREEN_RES_X-1,31);	//Background
	
	myLCD.setBackColor(HEADER_COLOUR);	//NAME
	myLCD.setColor(FONT_COLOUR);
	myLCD.setFont(Arial_round);
	myLCD.print(F("BAR LIGHT CONTROL"), CENTER, 7);

	//Help Button
	myLCD.setBackColor(BASE_COLOUR_2);
	myLCD.setColor(BASE_COLOUR_2);
	myLCD.fillRoundRect(SCREEN_RES_X-33, 0, SCREEN_RES_X-1, 31);
	myLCD.setColor(FONT_COLOUR);
	myLCD.print(F("?"), SCREEN_RES_X-24, 7);
	myLCD.drawRoundRect(SCREEN_RES_X-33, 0, SCREEN_RES_X-1, 31);
	
	//Line
	myLCD.setColor(BASE_COLOUR);
	myLCD.fillRect(0,32,SCREEN_RES_X-1,34);
	
	//Current settings
	myLCD.setColor(FONT_COLOUR);
	myLCD.setBackColor(BACKGROUND_COLOUR);
	myLCD.setFont(SmallFont);
	myLCD.print(F("Current setting:"), 5, 42);
	myLCD.fillRect(134,35,134,59);
	myLCD.print(currentSetting, 140, 42);
	myLCD.fillRect(0,60, SCREEN_RES_X-1,61);
	
	//========== BUTTONS ====================
	//========================================
	myLCD.setColor(BASE_COLOUR);
	myLCD.fillRoundRect(10,80, 240,115); //Button 1
	myLCD.setColor(FONT_COLOUR);
	myLCD.drawRoundRect(10,80, 240,115);
	myLCD.setBackColor(BASE_COLOUR);
	myLCD.setFont(arial_bold);
	myLCD.print(F("Light Scenes"), 30,90);
	
	myLCD.setColor(BASE_COLOUR);
	myLCD.fillRoundRect(245,80, 310,115); //Button 1
	myLCD.setColor(FONT_COLOUR);
	myLCD.drawRoundRect(245,80, 310,115);

	if(stepsUpDown==0) {
		myLCD.setColor(FONT_COLOUR);
		myLCD.setBackColor(BASE_COLOUR);
		myLCD.setFont(SmallFont);
		myLCD.print(F("Up"), 270,90);
	}
	else if(stepsUpDown>=1) {
		myLCD.setColor(FONT_COLOUR);
		myLCD.setBackColor(BASE_COLOUR);
		myLCD.setFont(SmallFont);
		myLCD.print(F("UpDown"), 255,90);
	}
	
	//========== SLIDERS ====================
	//========================================
	//Fading Speed Checkbox
		myLCD.setFont(arial_bold);
	myLCD.setBackColor(BACKGROUND_COLOUR);
	myLCD.print(F("Fading:"), SLIDER_X,SLIDER1_Y-SLIDER_FONT_Y);
	myLCD.setFont(Symbols_32); //Symbol font for checkbox
	if (fadeSwitch==0) {          //If fadeing off
		myLCD.setColor(VGA_RED);
		myLCD.print(F("A"), 120,118); //Crossed
		fadeDuration = 0;
	}
	else {                     //If Fading on
		myLCD.setColor(VGA_GREEN);
		myLCD.print(F("B"), 120,118); //Checked
	}
	
	//========== Fade speed text ==========
	myLCD.setFont(arial_bold);
	myLCD.setColor(FONT_COLOUR);
	myLCD.print(F("Speed:"), 160,SLIDER1_Y-SLIDER_FONT_Y);

	//========== Fade Slider ==========
		if(fadeSwitch==0) {
			myLCD.print(F("Off"), 265,SLIDER1_Y-SLIDER_FONT_Y); //Print off if value is 0
			myLCD.setColor(SLIDEROFF_COLOUR); // Grey slider if off
			myLCD.fillRect(SLIDER_X,SLIDER1_Y, SLIDER_X+10,SLIDER1_Y+SLIDER_HEIGHT);
			myLCD.drawRect(SLIDER_X,SLIDER1_Y, SLIDER_X+SLIDER_LENGTH,SLIDER1_Y+SLIDER_HEIGHT);
		}
		else {
			myLCD.print(F("   "),255,SLIDER1_Y-SLIDER_FONT_Y);
			if (fadeDuration>0){
				myLCD.printNumF((float)fadeDuration/1000,1,250,SLIDER1_Y-SLIDER_FONT_Y);	//TODO Allign nicely
				myLCD.print(F("s"), 300,SLIDER1_Y-SLIDER_FONT_Y); // print value in seconds
			} else {
				myLCD.print(F("Off"), 265,SLIDER1_Y-SLIDER_FONT_Y); //Print off if value is 0
			}
			
			//Draw bar with current values
			myLCD.setColor(FONT_COLOUR); 
			myLCD.drawRect(SLIDER_X,SLIDER1_Y, SLIDER_X+SLIDER_LENGTH,SLIDER1_Y+SLIDER_HEIGHT);  //Draw Box arround silder
			myLCD.fillRect(xFadeSlid,SLIDER1_Y,(xFadeSlid+10),SLIDER1_Y+SLIDER_HEIGHT); // Positioner
			myLCD.setColor(SLIDER_COLOUR);
			if (xFadeSlid>SLIDER_X) myLCD.fillRect(SLIDER_X+1, SLIDER1_Y+1, (xFadeSlid-1), SLIDER1_Y+SLIDER_HEIGHT-1); //Left bit is yellow
			myLCD.setColor(BACKGROUND_COLOUR);	//Chekc if that works
			if (xFadeSlid<SLIDER_LENGTH-1) myLCD.fillRect((xFadeSlid+11), SLIDER1_Y+1, SLIDER_X+SLIDER_LENGTH-1, SLIDER1_Y+SLIDER_HEIGHT-1); //Right bit is black
		}

	//========== Steptime Slider ==========
		myLCD.setColor(FONT_COLOUR);
		myLCD.print(F("Step Time:"), SLIDER_X,SLIDER2_Y-SLIDER_FONT_Y);
		
		if(xStepSlid<300) {
			myLCD.printNumF((float)stepDuration/1000,1,247,SLIDER2_Y-SLIDER_FONT_Y);
			myLCD.print(F("s"), 300,SLIDER2_Y-SLIDER_FONT_Y); // print value in seconds
		}
		else myLCD.print(F("Freeze"), 217,SLIDER2_Y-SLIDER_FONT_Y); // print value in seconds
		
		//Draw bar with current values
		myLCD.drawRect(SLIDER_X,SLIDER2_Y, SLIDER_X+SLIDER_LENGTH,SLIDER2_Y+SLIDER_HEIGHT);  //Draw Box arround silder
		myLCD.fillRect(xStepSlid,SLIDER2_Y,(xStepSlid+10),SLIDER2_Y+SLIDER_HEIGHT); // Positioner
		myLCD.setColor(SLIDER_COLOUR);
		if (xStepSlid>SLIDER_X) myLCD.fillRect(SLIDER_X+1, SLIDER2_Y+1, (xStepSlid-1), SLIDER2_Y+SLIDER_HEIGHT-1);
		myLCD.setColor(BACKGROUND_COLOUR);
		if (xStepSlid<SLIDER_LENGTH-1) myLCD.fillRect((xStepSlid+11), SLIDER2_Y+1, SLIDER_X+SLIDER_LENGTH-1, SLIDER2_Y+SLIDER_HEIGHT-1);

	//========== Brightness Slider ==========
		myLCD.setColor(FONT_COLOUR);
		myLCD.print(F("Brightness:"), SLIDER_X,SLIDER3_Y-SLIDER_FONT_Y);
		myLCD.print(String(map(brightness,0,255,0,100))+"%", 247,SLIDER3_Y-SLIDER_FONT_Y);
		
		//Draw bar with current values
		myLCD.drawRect(SLIDER_X,SLIDER3_Y, SLIDER_X+SLIDER_LENGTH,SLIDER3_Y+SLIDER_HEIGHT);  //Draw Box arround silder
		myLCD.fillRect(xBrightSlid,SLIDER3_Y,(xBrightSlid+10),SLIDER3_Y+SLIDER_HEIGHT); // Positioner
		myLCD.setColor(SLIDER_COLOUR);
		if (xBrightSlid>SLIDER_X) myLCD.fillRect(SLIDER_X+1, SLIDER3_Y+1, (xBrightSlid-1), SLIDER3_Y+SLIDER_HEIGHT-1);
		myLCD.setColor(BACKGROUND_COLOUR);
		if (xBrightSlid<SLIDER_LENGTH-1) myLCD.fillRect((xBrightSlid+11), SLIDER3_Y+1, SLIDER_X+SLIDER_LENGTH-1, SLIDER3_Y+SLIDER_HEIGHT-1);
	
	
	//========== COLOURS ====================
	//========================================
	myLCD.setColor(FONT_COLOUR);
	myLCD.print(F("Colours"), 340,123);
	myLCD.drawRect(317,120,SCREEN_RES_X-1,SCREEN_RES_Y-1);	//Box around this section
	
	#define CP_X		355
	#define CP_Y		160
	#define CP_SIZE		40
	#define CP_SPACE	10
	#define CP_SPACE_Y	20
	#define CP_SPACE_F	15
	myLCD.setFont(SmallFont);
	myLCD.print(F("1"), CP_X+CP_SIZE/2-4,CP_Y-CP_SPACE_F);		//Colour 1
	myLCD.print(F("2"), CP_X+CP_SIZE+CP_SIZE/2+CP_SPACE-4,CP_Y-CP_SPACE_F);		//Colour 2
	myLCD.print(F("3"), CP_X+CP_SIZE/2-4,CP_Y+CP_SIZE+CP_SPACE_Y-CP_SPACE_F);		//Colour 3
	myLCD.print(F("4"), CP_X+CP_SIZE+CP_SIZE/2+CP_SPACE-4,CP_Y+CP_SIZE+CP_SPACE_Y-CP_SPACE_F);		//Colour 4

	myLCD.setColor(getScreenColour(guiColourID[0]));
	myLCD.fillRect(CP_X,CP_Y,CP_X+CP_SIZE,CP_Y+CP_SIZE);
	myLCD.setColor(FONT_COLOUR);
	myLCD.drawRect(CP_X,CP_Y,CP_X+CP_SIZE,CP_Y+CP_SIZE);
	myLCD.setColor(getScreenColour(guiColourID[1]));
	myLCD.fillRect(CP_X+CP_SIZE+CP_SPACE,CP_Y,CP_X+CP_SIZE*2+CP_SPACE,CP_Y+CP_SIZE);
	myLCD.setColor(FONT_COLOUR);
	myLCD.drawRect(CP_X+CP_SIZE+CP_SPACE,CP_Y,CP_X+CP_SIZE*2+CP_SPACE,CP_Y+CP_SIZE);
	myLCD.setColor(getScreenColour(guiColourID[2]));	
	myLCD.fillRect(CP_X,CP_Y+CP_SIZE+CP_SPACE_Y,CP_X+CP_SIZE,CP_Y+CP_SIZE*2+CP_SPACE_Y);
	myLCD.setColor(FONT_COLOUR);
	myLCD.drawRect(CP_X,CP_Y+CP_SIZE+CP_SPACE_Y,CP_X+CP_SIZE,CP_Y+CP_SIZE*2+CP_SPACE_Y);
	myLCD.setColor(getScreenColour(guiColourID[3]));
	myLCD.fillRect(CP_X+CP_SIZE+CP_SPACE,CP_Y+CP_SIZE+CP_SPACE_Y,CP_X+CP_SIZE*2+CP_SPACE,CP_Y+CP_SIZE*2+CP_SPACE_Y);
	myLCD.setColor(FONT_COLOUR);
	myLCD.drawRect(CP_X+CP_SIZE+CP_SPACE,CP_Y+CP_SIZE+CP_SPACE_Y,CP_X+CP_SIZE*2+CP_SPACE,CP_Y+CP_SIZE*2+CP_SPACE_Y);	
	
	
	//========== MUSIC CONTROL ====================
	//========================================
	#define MB_X 370
	#define MB_Y 85
	#define MB_LENGTH 50
	#define MB_HEIGHT 25
	#define MB_TEX_X  10
	#define MB_TEX_Y  7
	
	myLCD.setColor(FONT_COLOUR);
	myLCD.print(F("Music Control:"), 335,67);
	myLCD.setColor(BASE_COLOUR);
	myLCD.fillRoundRect(MB_X,MB_Y,MB_X+MB_LENGTH,MB_Y+MB_HEIGHT);	//Box around this section
	myLCD.setColor(FONT_COLOUR);
	myLCD.drawRoundRect(MB_X,MB_Y,MB_X+MB_LENGTH,MB_Y+MB_HEIGHT);	//Box around this section
	myLCD.setBackColor(BASE_COLOUR);
	myLCD.print(F("    "), MB_X+MB_TEX_X,MB_Y+MB_TEX_Y);
	if(musicSen==0)	 	 myLCD.print(F("OFF"), MB_X+MB_TEX_X+4,MB_Y+MB_TEX_Y);
	else if(musicSen==1) myLCD.print(F("LOW"), MB_X+MB_TEX_X+4,MB_Y+MB_TEX_Y);
	else if(musicSen==2) myLCD.print(F("MID"), MB_X+MB_TEX_X+4,MB_Y+MB_TEX_Y);
	else if(musicSen==3) myLCD.print(F("HIGH"), MB_X+MB_TEX_X,MB_Y+MB_TEX_Y);
}

void LED_TFT::drawColourPicker() {
	myLCD.clrScr(); // Clears the screen
	currentPage = 2;		//Set right page
	
	//====== Header
	myLCD.setColor(HEADER_COLOUR);
	myLCD.fillRect(0,0,SCREEN_RES_X-1,32);	//Background
	
	myLCD.setBackColor(HEADER_COLOUR);	//NAME
	myLCD.setColor(FONT_COLOUR);
	myLCD.setFont(Arial_round);
	myLCD.print(F("COLOUR CHOOSER"), CENTER, 7);
		
	//Line
	myLCD.setColor(BASE_COLOUR);
	myLCD.fillRect(0,32,SCREEN_RES_X-1,34);
	
	//Back button
	myLCD.setBackColor(BASE_COLOUR_2);
	myLCD.setColor(FONT_COLOUR);
	myLCD.setFont(Symbols_32);
	myLCD.print(F("?"), LEFT, 1);
	myLCD.drawRect(0, 0, 32, 32);
	
	//========== COLOUR PALATE ==============
	#define C_X	 		30
	#define C_Y	 		70
	#define C_SIZE		60
	#define C_SPACE_X	30
	#define C_SPACE_Y	50
	#define C_TEX		13
	
	myLCD.setFont(SmallFont);
	myLCD.setColor(FONT_COLOUR);
	myLCD.setBackColor(BACKGROUND_COLOUR);
	myLCD.print(F("Black"), C_X,C_Y-C_TEX);		//Colour 0
	myLCD.print(F("White"), C_X+C_SIZE*1+C_SPACE_X*1,C_Y-C_TEX);		//Colour 1
	myLCD.print(F("Green"), C_X+C_SIZE*2+C_SPACE_X*2,C_Y-C_TEX);		//Colour 2
	myLCD.print(F("Red"), C_X+C_SIZE*3+C_SPACE_X*3,C_Y-C_TEX);			//Colour 3
	myLCD.print(F("Blue"), C_X+C_SIZE*4+C_SPACE_X*4,C_Y-C_TEX);		//Colour 4
	myLCD.print(F("Yellow"), C_X,C_Y+C_SIZE+C_SPACE_Y-C_TEX);		//Colour 5
	myLCD.print(F("Cyan"), C_X+C_SIZE*1+C_SPACE_X*1,C_Y+C_SIZE+C_SPACE_Y-C_TEX);		//Colour 6
	myLCD.print(F("Magent"), C_X+C_SIZE*2+C_SPACE_X*2,C_Y+C_SIZE+C_SPACE_Y-C_TEX);		//Colour 7
	myLCD.print(F("Orange"), C_X+C_SIZE*3+C_SPACE_X*3,C_Y+C_SIZE+C_SPACE_Y-C_TEX);		//Colour 8
	myLCD.print(F("Lightblue"), C_X+C_SIZE*4+C_SPACE_X*4,C_Y+C_SIZE+C_SPACE_Y-C_TEX);	//Colour 9
	//Line one
	myLCD.setColor(VGA_BLACK);	
	myLCD.fillRect(C_X,C_Y,C_X+C_SIZE,C_Y+C_SIZE);
	myLCD.setColor(VGA_WHITE);
	myLCD.drawRect(C_X,C_Y,C_X+C_SIZE,C_Y+C_SIZE);	//White line around black field
	myLCD.fillRect(C_X+C_SIZE*1+C_SPACE_X*1,C_Y,C_X+C_SIZE*2+C_SPACE_X*1,C_Y+C_SIZE);
	myLCD.setColor(VGA_GREEN);	
	myLCD.fillRect(C_X+C_SIZE*2+C_SPACE_X*2,C_Y,C_X+C_SIZE*3+C_SPACE_X*2,C_Y+C_SIZE);
	myLCD.setColor(VGA_RED);
	myLCD.fillRect(C_X+C_SIZE*3+C_SPACE_X*3,C_Y,C_X+C_SIZE*4+C_SPACE_X*3,C_Y+C_SIZE);	
	myLCD.setColor(VGA_BLUE);
	myLCD.fillRect(C_X+C_SIZE*4+C_SPACE_X*4,C_Y,C_X+C_SIZE*5+C_SPACE_X*4,C_Y+C_SIZE);	
	
	//Line two
	myLCD.setColor(VGA_YELLOW);	
	myLCD.fillRect(C_X,C_Y+C_SIZE+C_SPACE_Y,C_X+C_SIZE,C_Y+C_SIZE*2+C_SPACE_Y);
	myLCD.setColor(VGA_CYAN);
	myLCD.fillRect(C_X+C_SIZE*1+C_SPACE_X*1,C_Y+C_SIZE+C_SPACE_Y,C_X+C_SIZE*2+C_SPACE_X*1,C_Y+C_SIZE*2+C_SPACE_Y);
	myLCD.setColor(VGA_MAGENTA);	
	myLCD.fillRect(C_X+C_SIZE*2+C_SPACE_X*2,C_Y+C_SIZE+C_SPACE_Y,C_X+C_SIZE*3+C_SPACE_X*2,C_Y+C_SIZE*2+C_SPACE_Y);
	myLCD.setColor(VGA_ORANGE);
	myLCD.fillRect(C_X+C_SIZE*3+C_SPACE_X*3,C_Y+C_SIZE+C_SPACE_Y,C_X+C_SIZE*4+C_SPACE_X*3,C_Y+C_SIZE*2+C_SPACE_Y);	
	myLCD.setColor(VGA_LIGHTBLUE);
	myLCD.fillRect(C_X+C_SIZE*4+C_SPACE_X*4,C_Y+C_SIZE+C_SPACE_Y,C_X+C_SIZE*5+C_SPACE_X*4,C_Y+C_SIZE*2+C_SPACE_Y);	
	
}

void LED_TFT::drawFileExplorer() {
	myLCD.clrScr();
	//Set right page:
	currentPage = 1;
	
	//Delay So click on button doesnt do anything on new page
	delay(50);
	//Serial.println(strcmp(browser.getCurrentPath(),"LightScences")!=0);
	browser.changeDir("/LightScences"); //Standard directory
  
	//====== Header
	myLCD.setColor(HEADER_COLOUR);
	myLCD.fillRect(0,0,SCREEN_RES_X-1,32);	//Background
	
	myLCD.setBackColor(BACKGROUND_COLOUR);	//NAME
	myLCD.setColor(FONT_COLOUR);
	drawCurrentPath();
	
	//Back button
	myLCD.setBackColor(BASE_COLOUR_2);
	myLCD.setColor(FONT_COLOUR);
	myLCD.setFont(Symbols_32);
	myLCD.print(F("?"), LEFT, 1);
	myLCD.drawRect(0, 0, 32, 32);
	
	//Line
	myLCD.setColor(BASE_COLOUR);
	myLCD.fillRect(0, 33, SCREEN_RES_X-1, 35);
	
	//Scroll up Button
	myLCD.setColor(FONT_COLOUR);
	myLCD.print(F("="), RIGHT, 36);
	myLCD.drawRect(SCREEN_RES_X-33, 36, SCREEN_RES_X-1, 68);
	
	//Scroll down Button
	myLCD.print(F(">"), RIGHT, SCREEN_RES_Y-33);
	myLCD.drawRect(SCREEN_RES_X-33, SCREEN_RES_Y-33, SCREEN_RES_X-1, SCREEN_RES_Y-1);
	
	//Bottom Line
	myLCD.fillRect(0, SCREEN_RES_Y-33, SCREEN_RES_X-1, SCREEN_RES_Y-33);
	
	myLCD.setFont(arial_bold);
	myLCD.setBackColor(BACKGROUND_COLOUR);
	myLCD.print(F("Random Colours"), CENTER, SCREEN_RES_Y-24);
	
	
	
	//FileList
	drawFileList();
  
}

void LED_TFT::drawHelpMenu() {
	myLCD.clrScr(); // Clears the screen
	currentPage = 3;		//Set right page
	
	//====== Header
	myLCD.setColor(HEADER_COLOUR);
	myLCD.fillRect(0,0,SCREEN_RES_X-1,32);	//Background
	
	myLCD.setBackColor(HEADER_COLOUR);	//NAME
	myLCD.setColor(FONT_COLOUR);
	myLCD.setFont(Arial_round);
	myLCD.print(F("HELP MENU"), CENTER, 7);
		
	//Line
	myLCD.setColor(BASE_COLOUR);
	myLCD.fillRect(0,32,SCREEN_RES_X-1,34);
	
	//Back button
	myLCD.setBackColor(BASE_COLOUR_2);
	myLCD.setColor(FONT_COLOUR);
	myLCD.setFont(Symbols_32);
	myLCD.print(F("?"), LEFT, 1);
	myLCD.drawRect(0, 0, 32, 32);
		
	myLCD.setBackColor(BACKGROUND_COLOUR);
	myLCD.setColor(FONT_COLOUR);
	myLCD.setFont(SmallFont);
	myLCD.print(F("1.Button Light Scenes: Auswahl verschiedener Einstellungen"),5,40);
	myLCD.print(F("2.Button Up/UpDown: Legt fest ob Szenen in eine Richtung"),5,40+16);
	myLCD.print(F("  oder hoch und runter gespielt wird"),5,40+16*2);
	myLCD.print(F("3.Music Control: Legt die Sensibilitaet der"),5,40+16*3);
	myLCD.print(F("  Musiksteuerung fest. Kann unten  verstellt werden"),5,40+16*4);
	myLCD.print(F("4.Colours: Auf Feld druecken um Farbe zu aendern"),5,40+16*5);
	myLCD.print(F("5.Fading: An oder Abschalten"),5,40+16*6);
	myLCD.print(F("6.Slider: Zeit/Helligkeit kann eingstellt werden"),5,40+16*7);
	myLCD.print(F("7.Steptime: Wenn ganz voll bleibt das licht gleich"),5,40+16*8);
	myLCD.print(F("  ausser Music control ist an"),5,40+16*9);
	
	myLCD.fillRect(0,39+16*10,SCREEN_RES_X-1,39+16*10);
	//MUSIC THRESHOLD CONTROL
	#define MIC_Y	22
	#define MIC_X	SCREEN_RES_X/2
	myLCD.setFont(arial_bold);
	
	myLCD.setBackColor(BACKGROUND_COLOUR);
	myLCD.print(F("Music-"),5,SCREEN_RES_Y-MIC_Y*3+10);
	myLCD.print(F("Cntrl:"),5,SCREEN_RES_Y-MIC_Y*2+10);
	myLCD.print(F("Low:"),MIC_X-110,SCREEN_RES_Y-MIC_Y);
	myLCD.print(F("Mid:"),MIC_X-110,SCREEN_RES_Y-MIC_Y*2);
	myLCD.print(F("High:"),MIC_X-126,SCREEN_RES_Y-MIC_Y*3);
	
	
	myLCD.setBackColor(BASE_COLOUR_2);
	myLCD.print(F("-"),MIC_X-43,SCREEN_RES_Y-MIC_Y);
	myLCD.print(F("+"),MIC_X+30,SCREEN_RES_Y-MIC_Y);
	myLCD.print(F("-"),MIC_X-43,SCREEN_RES_Y-MIC_Y*2);
	myLCD.print(F("+"),MIC_X+30,SCREEN_RES_Y-MIC_Y*2);
	myLCD.print(F("-"),MIC_X-43,SCREEN_RES_Y-MIC_Y*3);
	myLCD.print(F("+"),MIC_X+30,SCREEN_RES_Y-MIC_Y*3);
	
	myLCD.setBackColor(BACKGROUND_COLOUR);
	myLCD.printNumI(MIC_THRES_LOW,CENTER,SCREEN_RES_Y-MIC_Y,3);
	myLCD.printNumI(MIC_THRES_MID,CENTER,SCREEN_RES_Y-MIC_Y*2,3);
	myLCD.printNumI(MIC_THRES_HIGH,CENTER,SCREEN_RES_Y-MIC_Y*3,3);
	
}

//============ FILEBROWSER FUNCTIONS ===========
void LED_TFT::drawFileList() {
  myLCD.setFont(arial_bold);
  myLCD.setBackColor(BACKGROUND_COLOUR);
  myLCD.setColor(FONT_COLOUR);
  numFiles = MAX_LIST_ITEMS;
  if((browser.getNumOfFiles()-pageIndex*MAX_LIST_ITEMS) < MAX_LIST_ITEMS) {
    numFiles = browser.getNumOfFiles() - pageIndex*MAX_LIST_ITEMS;
  }

  myLCD.print(F("../"), 5, 45);
  for (int i = 0; i < numFiles; i++) {
    //TODO Check for left over files
    myLCD.print(F("                             "), 5, FILELIST_Y_TOP + i * FILELIST_LINE_SIZE);
    myLCD.print(browser.CURRENT_FILE_LIST[i+ pageIndex*MAX_LIST_ITEMS], 5, FILELIST_Y_TOP + i * FILELIST_LINE_SIZE);
  }
  
  //Wipe the rest of the list
  for (int i = numFiles; i < MAX_LIST_ITEMS; i++) {
    myLCD.print(F("                             "), 5, FILELIST_Y_TOP + i * FILELIST_LINE_SIZE);
  }

  drawCurrentPath();
  
}

void LED_TFT::drawCurrentPath() {
	//Clear the old path
	myLCD.setColor(HEADER_COLOUR);
	myLCD.fillRect(33,0,SCREEN_RES_X-1,32);	//Background
	myLCD.setBackColor(HEADER_COLOUR);
	myLCD.setColor(FONT_COLOUR);
	myLCD.setFont(SmallFont);
	myLCD.print(browser.getCurrentPath(), CENTER, 10);
}

void LED_TFT::fileClick(uint8_t index) {
  uint8_t n = browser.onClick((uint8_t)(index + pageIndex*MAX_LIST_ITEMS), scene, numSteps, currentSetting); //pass pointer of scene! Check if pointers are correctly passed and variables modified correctly 
  if(n==2) {
    drawFileList();
  }
  else if (n==1) {
	lightMode = 0;	//Here the normal fadeLED function should be used
	myLCD.clrScr(); // Clears the screen
	drawLightControl();
  }
  else {
    //OPTIONAL: error to be implemented -> Show warning
	//showWarning();
  }
    
}

//============= LIGHT CONTROL Funtion ===========
void LED_TFT::sliderControl() {
    if (fadeSwitch!=0) {
		// Area of the Fade-Slider
		if( (y>=SLIDER1_Y) && (y<=SLIDER1_Y+SLIDER_HEIGHT) && (x<=SLIDER_X+SLIDER_LENGTH) ) {
				xFadeSlid=x; // Stores the X value where the screen has been pressed in to variable xSlid
			if (xFadeSlid<=SLIDER_X) { // Confines the area of the slider to be above SLIDER_X pixels
				xFadeSlid=SLIDER_X;
			}
			if (xFadeSlid>=SLIDER_LENGTH){ /// Confines the area of the slider to be under 310 pixels
				xFadeSlid=SLIDER_LENGTH; 
			} 

			//Print new value of speed
			myLCD.setBackColor(BACKGROUND_COLOUR);
			myLCD.setColor(FONT_COLOUR);
			myLCD.setFont(arial_bold);
			if (xFadeSlid==SLIDER_X){
				fadeDuration = 0;
				myLCD.print(F("Off"), 265,SLIDER1_Y-SLIDER_FONT_Y); //Print off if value is 0
			} else if(xFadeSlid<=SLIDER_LENGTH/2){
				fadeDuration = map(xFadeSlid,SLIDER_X,SLIDER_LENGTH/2,0,300);	//Map the fader position to 
				myLCD.print(F("   "),255,SLIDER1_Y-SLIDER_FONT_Y);
				myLCD.printNumF((float)fadeDuration/1000,1,250,SLIDER1_Y-SLIDER_FONT_Y);	//TODO Allign nicely
				myLCD.print(F("s"), 300,SLIDER1_Y-SLIDER_FONT_Y); // print value in seconds
			} else {
				fadeDuration = map(xFadeSlid,SLIDER_LENGTH/2+1,SLIDER_LENGTH,300,5000);	//Map the fader position to 
				myLCD.print(F("   "),255,SLIDER1_Y-SLIDER_FONT_Y);
				myLCD.printNumF((float)fadeDuration/1000,1,250,SLIDER1_Y-SLIDER_FONT_Y);	//TODO Allign nicely
				myLCD.print(F("s"), 300,SLIDER1_Y-SLIDER_FONT_Y); // print value in seconds
			}
		
			// Draws the positioners
			myLCD.setColor(FONT_COLOUR);
			myLCD.fillRect(xFadeSlid,SLIDER1_Y,(xFadeSlid+10),SLIDER1_Y+SLIDER_HEIGHT); // Positioner
			myLCD.setColor(SLIDER_COLOUR);
			if (xFadeSlid>SLIDER_X) myLCD.fillRect(SLIDER_X+1, SLIDER1_Y+1, (xFadeSlid-1), SLIDER1_Y+SLIDER_HEIGHT-1);
			myLCD.setColor(BACKGROUND_COLOUR);
			if (xFadeSlid<SLIDER_LENGTH-1) myLCD.fillRect((xFadeSlid+11), SLIDER1_Y+1, SLIDER_X+SLIDER_LENGTH-1, SLIDER1_Y+SLIDER_HEIGHT-1);
			myLCD.setColor(FONT_COLOUR);
			myLCD.drawRect(SLIDER_X,SLIDER1_Y, SLIDER_X+SLIDER_LENGTH,SLIDER1_Y+SLIDER_HEIGHT);  //Draw Box arround silder
			return;
		}
	}
    
	// Area of the StepTime-Slider	//TODO: Finer Timings in lower times
	if( (y>=SLIDER2_Y) && (y<=SLIDER2_Y+SLIDER_HEIGHT) && (x<=SLIDER_X+SLIDER_LENGTH)) {
		xStepSlid=x; // Stores the X value where the screen has been pressed in to variable xSlid
		if (xStepSlid<SLIDER_X) { // Confines the area of the slider to be above 10 pixels
			xStepSlid=SLIDER_X;
		}
		if (xStepSlid>SLIDER_LENGTH){ /// Confines the area of the slider to be under 310 pixels
			xStepSlid=SLIDER_LENGTH;
		}
		
		//======== Calculate the stepTime ======== 
		myLCD.setBackColor(BACKGROUND_COLOUR);
		myLCD.setColor(FONT_COLOUR);
		myLCD.setFont(arial_bold);
		if (xStepSlid==SLIDER_X){
			stepDuration = 10;
			myLCD.print(F("      "),217,SLIDER2_Y-SLIDER_FONT_Y);
			myLCD.printNumF((float)stepDuration/1000,1,247,SLIDER2_Y-SLIDER_FONT_Y);
			myLCD.print(F("s"), 300,SLIDER2_Y-SLIDER_FONT_Y); // print value in seconds
		}
		else if(xStepSlid==300) {
			stepDuration = 4294960000;	//If slider is full then leave step for long time
			myLCD.print(F("      "),217,SLIDER2_Y-SLIDER_FONT_Y);
			myLCD.print(F("Freeze"), 217,SLIDER2_Y-SLIDER_FONT_Y); // print value in seconds
		} 
		else if(xStepSlid<=SLIDER_LENGTH/2){
			stepDuration = map(xStepSlid,SLIDER_X,SLIDER_LENGTH/2,10,1000);	//Map the fader position to 
			myLCD.print(F("      "),217,SLIDER2_Y-SLIDER_FONT_Y);
			myLCD.printNumF((float)stepDuration/1000,1,247,SLIDER2_Y-SLIDER_FONT_Y);
			myLCD.print(F("s"), 300,SLIDER2_Y-SLIDER_FONT_Y); // print value in seconds
		}
		else {
			stepDuration = map(xStepSlid,SLIDER_LENGTH/2+1,SLIDER_LENGTH,1000,5000); //Map the fader position to 
			myLCD.print(F("      "),217,SLIDER2_Y-SLIDER_FONT_Y);
			myLCD.printNumF((float)stepDuration/1000,1,247,SLIDER2_Y-SLIDER_FONT_Y);
			myLCD.print(F("s"), 300,SLIDER2_Y-SLIDER_FONT_Y); // print value in seconds
		}
		
		// Draws the positioners
		myLCD.drawRect(SLIDER_X,SLIDER2_Y, SLIDER_X+SLIDER_LENGTH,SLIDER2_Y+SLIDER_HEIGHT);  //Draw Box arround silder
		myLCD.fillRect(xStepSlid,SLIDER2_Y,(xStepSlid+10),SLIDER2_Y+SLIDER_HEIGHT); // Positioner
		myLCD.setColor(SLIDER_COLOUR);
		if (xStepSlid>SLIDER_X) myLCD.fillRect(SLIDER_X+1, SLIDER2_Y+1, (xStepSlid-1), SLIDER2_Y+SLIDER_HEIGHT-1);
		myLCD.setColor(BACKGROUND_COLOUR);
		if (xStepSlid<SLIDER_LENGTH-1) myLCD.fillRect((xStepSlid+11), SLIDER2_Y+1, SLIDER_X+SLIDER_LENGTH-1, SLIDER2_Y+SLIDER_HEIGHT-1);
		return;
		
	}
	
	//======== Area of the Brightness-Slider ======== 
	if( (y>=SLIDER3_Y) && (y<=SLIDER3_Y+SLIDER_HEIGHT) && (x<=SLIDER_X+SLIDER_LENGTH)) {
		xBrightSlid=x; // Stores the X value where the screen has been pressed in to variable xSlid
		if (xBrightSlid<SLIDER_X) { // Confines the area of the slider to be above 10 pixels
			xBrightSlid=SLIDER_X;
		}
		if (xBrightSlid>SLIDER_LENGTH){ /// Confines the area of the slider to be under 310 pixels
			xBrightSlid=SLIDER_LENGTH;
		}
		
		//======== Calculate the brightness ======== 
		myLCD.setBackColor(BACKGROUND_COLOUR);
		myLCD.setColor(FONT_COLOUR);
		myLCD.setFont(arial_bold);
		if (xBrightSlid==SLIDER_X){
			brightness = 0;	
		}
		else {
			brightness = map(xBrightSlid,SLIDER_X,SLIDER_LENGTH,0,255); //Map the fader position to 
		}
		
		myLCD.print(F("    "),247,SLIDER3_Y-SLIDER_FONT_Y);
		myLCD.print(F("Brightness:"), SLIDER_X,SLIDER3_Y-SLIDER_FONT_Y);
		myLCD.print(String(map(brightness,0,255,0,100))+"%", 247,SLIDER3_Y-SLIDER_FONT_Y);
		
		// Draws the positioners
		myLCD.drawRect(SLIDER_X,SLIDER3_Y, SLIDER_X+SLIDER_LENGTH,SLIDER3_Y+SLIDER_HEIGHT);  //Draw Box arround silder
		myLCD.fillRect(xBrightSlid,SLIDER3_Y,(xBrightSlid+10),SLIDER3_Y+SLIDER_HEIGHT); // Positioner
		myLCD.setColor(SLIDER_COLOUR);
		if (xBrightSlid>SLIDER_X) myLCD.fillRect(SLIDER_X+1, SLIDER3_Y+1, (xBrightSlid-1), SLIDER3_Y+SLIDER_HEIGHT-1);
		myLCD.setColor(BACKGROUND_COLOUR);
		if (xBrightSlid<SLIDER_LENGTH-1) myLCD.fillRect((xBrightSlid+11), SLIDER3_Y+1, SLIDER_X+SLIDER_LENGTH-1, SLIDER3_Y+SLIDER_HEIGHT-1);	
		return;
	}
}


//============== TOUCH SCREEN FUNCTIONS==========
void LED_TFT::handleTouchLedSettings() {
	//The silder control sets the slider
	sliderControl();
	// If we press the File explorer Button 
	if ((x>=10) && (x<=240) && (y>=80) && (y<=115)) {
		myLCD.clrScr();
		drawFileExplorer();
		return;
	}
	
	else if ((x>=245) && (x<=310) && (y>=80) && (y<=115)) {
		if(stepsUpDown==0) {
			stepsUpDown =1;
			myLCD.setColor(FONT_COLOUR);
			myLCD.setBackColor(BASE_COLOUR);
			myLCD.setFont(SmallFont);
			myLCD.print(F("      "), 255,90);
			myLCD.print(F("UpDown"), 255,90);
		}
		else if(stepsUpDown>=1) {
			stepsUpDown=0;
			myLCD.setColor(FONT_COLOUR);
			myLCD.setBackColor(BASE_COLOUR);
			myLCD.setFont(SmallFont);
			myLCD.print(F("      "), 255,90);
			myLCD.print(F("Up"), 270,90);
		}
		return;
	}

		  
	// If we press the Fading Button
	else if ((x>=120) && (x<=152) && (y>=120) && (y<=152)) {
		myLCD.setBackColor(BACKGROUND_COLOUR);
		myLCD.setFont(Symbols_32); //Symbol font for checkbox
		if (fadeSwitch==0) { //If fading is off turn on
			fadeDuration = fadeDurationBuf;
			fadeSwitch = 1;
			
			myLCD.setColor(VGA_GREEN);
			myLCD.print(F("B"), 120,118); //Checked box
			myLCD.setFont(arial_bold);
			myLCD.setColor(FONT_COLOUR);
			myLCD.print(F("    "),250,SLIDER1_Y-SLIDER_FONT_Y);
			if (fadeDuration>0) {	//Then update the fadeSlider
				myLCD.printNumF((float)fadeDuration/1000,1,250,SLIDER1_Y-SLIDER_FONT_Y);	//TODO Allign nicely
				myLCD.print(F("s"), 300,SLIDER1_Y-SLIDER_FONT_Y); // print value in seconds
				//Draw slider with last value
				myLCD.setColor(FONT_COLOUR); 
				myLCD.drawRect(SLIDER_X,SLIDER1_Y, SLIDER_X+SLIDER_LENGTH,SLIDER1_Y+SLIDER_HEIGHT);  //Draw Box arround silder
				myLCD.fillRect(xFadeSlid,SLIDER1_Y,(xFadeSlid+10),SLIDER1_Y+SLIDER_HEIGHT); // Positioner
				myLCD.setColor(SLIDER_COLOUR);
				if (xFadeSlid>SLIDER_X) myLCD.fillRect(SLIDER_X+1, SLIDER1_Y+1, (xFadeSlid-1), SLIDER1_Y+SLIDER_HEIGHT-1); //Left bit is yellow
				myLCD.setColor(VGA_BLACK);
				if (xFadeSlid<SLIDER_LENGTH) myLCD.fillRect((xFadeSlid+11), SLIDER1_Y+1, SLIDER_X+SLIDER_LENGTH-1, SLIDER1_Y+SLIDER_HEIGHT-1); //Right bit is black
			}
			else {	//If fade is of print info
				myLCD.print(F("Off"), 265,SLIDER1_Y-SLIDER_FONT_Y); //Print off if value is 0
			}
			return;
		}
		else if (fadeSwitch==1) { //If fading is on turn off
			fadeSwitch = 0;
			fadeDurationBuf = fadeDuration;
			fadeDuration = 0;
			
			myLCD.setColor(VGA_RED);
			myLCD.print(F("A"), 120,118); //Crossed box
			myLCD.setColor(SLIDEROFF_COLOUR);
			myLCD.drawRect(SLIDER_X,SLIDER1_Y, SLIDER_X+SLIDER_LENGTH,SLIDER1_Y+SLIDER_HEIGHT);	//Box around
			myLCD.fillRect(SLIDER_X,SLIDER1_Y, SLIDER_X+10,SLIDER1_Y+SLIDER_HEIGHT);
			myLCD.setColor(BACKGROUND_COLOUR);
			myLCD.fillRect(SLIDER_X+11, SLIDER1_Y+1, SLIDER_X+SLIDER_LENGTH-1, SLIDER1_Y+SLIDER_HEIGHT-1);	//Black box
			myLCD.setColor(FONT_COLOUR);
			myLCD.setFont(arial_bold);
			myLCD.print(F("    "),250,SLIDER1_Y-SLIDER_FONT_Y);
			myLCD.print(F("Off"), 265,SLIDER1_Y-SLIDER_FONT_Y); //Print off if value is 0
			return;
		}
	}

	//=== Colourpicker ====
	else if((x>=CP_X) && (x<=CP_X+CP_SIZE) && (y>=CP_Y) && (y<=CP_Y+CP_SIZE)) {	//Press on Colour 1
		changedColourID = 0;
		drawColourPicker();
		return;
	}
	else if((x>=CP_X+CP_SIZE+CP_SPACE) && (x<=CP_X+CP_SIZE*2+CP_SPACE) && (y>=CP_Y) && (y<=CP_Y+CP_SIZE)) {	//Press on Colour 2
		changedColourID = 1;
		drawColourPicker();
		return;
	}
	else if((x>=CP_X) && (x<=CP_X+CP_SIZE) && (y>=CP_Y+CP_SIZE+CP_SPACE_Y) && (y<=CP_Y+CP_SIZE*2+CP_SPACE_Y)) {	//Press on Colour 3
		changedColourID = 2;
		drawColourPicker();
		return;
	}
	else if((x>=CP_X+CP_SIZE+CP_SPACE) && (x<=CP_X+CP_SIZE*2+CP_SPACE) && (y>=CP_Y+CP_SIZE+CP_SPACE_Y) && (y<=CP_Y+CP_SIZE*2+CP_SPACE_Y)) {	//Press on Colour 4
		changedColourID = 3;
		drawColourPicker();
		return;
	}
	
	//Music Control
	else if ((x>=MB_X) && (x<=MB_X+MB_LENGTH) && (y>=MB_Y) && (y<=MB_Y+MB_HEIGHT)) {
		myLCD.setBackColor(BASE_COLOUR);
		myLCD.setColor(FONT_COLOUR);
		myLCD.setFont(SmallFont);
		myLCD.print(F("    "), MB_X+MB_TEX_X,MB_Y+MB_TEX_Y);
		//Serial.println(musicSen);
		if(musicSen==0) {
			myLCD.print(F("LOW"), MB_X+MB_TEX_X+4,MB_Y+MB_TEX_Y);
			musicSen = 1;
			micThreshold = MIC_THRES_LOW;	//TODO: Find correct values
			//Serial.print("From 0: "); Serial.println(musicSen);
			return;
		}
		else if(musicSen==1) {
			myLCD.print(F("MID"), MB_X+MB_TEX_X+4,MB_Y+MB_TEX_Y);
			musicSen = 2;
			//Serial.print("From 1: "); Serial.println(musicSen);
			micThreshold = MIC_THRES_MID;	//TODO: Find correct values
			return;
		}
		else if(musicSen==2) {
			 myLCD.print(F("HIGH"), MB_X+MB_TEX_X,MB_Y+MB_TEX_Y);
			musicSen = 3;
			//Serial.print("From 2: "); Serial.println(musicSen);
			micThreshold = MIC_THRES_HIGH;	//TODO: Find correct values
			return;
		}
		else if(musicSen==3) {
			 myLCD.print(F("OFF"), MB_X+MB_TEX_X+4,MB_Y+MB_TEX_Y);
			musicSen = 0;
			//Serial.print("From 3: "); Serial.println(musicSen);
			micThreshold = 9999;	//TODO: Find correct values
			return;
		}
		return;
	}
	
	//HelpMenu Button
	else if ( (x>=SCREEN_RES_X-33) && (x<=SCREEN_RES_X-1) && (y>=0) && (y<=32) ) {
		drawHelpMenu();
		return;
	}
	
}

void LED_TFT::handleTouchColourPicker() {
	//If back button is pressed 0, 0, 32, 32
	if ( (x >= 0) && (x <= 32) &&  (y >= 0) && (y <= 32)) {
		drawLightControl(); //Function to control the LEDs
		return;
    }
	
	else if( (y >= C_Y) && (y <= C_Y+C_SIZE) ) {
		if( (x >= C_X+C_SIZE*0+C_SPACE_X*0) && (x <= C_X+C_SIZE*1+C_SPACE_X*0) ) { guiColourID[changedColourID] = 0; drawLightControl(); return; }
		if( (x >= C_X+C_SIZE*1+C_SPACE_X*1) && (x <= C_X+C_SIZE*2+C_SPACE_X*1) ) { guiColourID[changedColourID] = 1; drawLightControl(); return; }
		if( (x >= C_X+C_SIZE*2+C_SPACE_X*2) && (x <= C_X+C_SIZE*3+C_SPACE_X*2) ) { guiColourID[changedColourID] = 2; drawLightControl(); return; }
		if( (x >= C_X+C_SIZE*3+C_SPACE_X*3) && (x <= C_X+C_SIZE*4+C_SPACE_X*3) ) { guiColourID[changedColourID] = 3; drawLightControl(); return; }
		if( (x >= C_X+C_SIZE*4+C_SPACE_X*4) && (x <= C_X+C_SIZE*5+C_SPACE_X*4) ) { guiColourID[changedColourID] = 4; drawLightControl(); return; }
	}
	
	else if( (y >= C_Y+C_SIZE+C_SPACE_Y) && (y <= C_Y+C_SIZE*2+C_SPACE_Y) ) {
		if( (x >= C_X+C_SIZE*0+C_SPACE_X*0) && (x <= C_X+C_SIZE*1+C_SPACE_X*0) ) { guiColourID[changedColourID] = 5; drawLightControl(); return; }
		if( (x >= C_X+C_SIZE*1+C_SPACE_X*1) && (x <= C_X+C_SIZE*2+C_SPACE_X*1) ) { guiColourID[changedColourID] = 6; drawLightControl(); return; }
		if( (x >= C_X+C_SIZE*2+C_SPACE_X*2) && (x <= C_X+C_SIZE*3+C_SPACE_X*2) ) { guiColourID[changedColourID] = 7; drawLightControl(); return; }
		if( (x >= C_X+C_SIZE*3+C_SPACE_X*3) && (x <= C_X+C_SIZE*4+C_SPACE_X*3) ) { guiColourID[changedColourID] = 8; drawLightControl(); return; }
		if( (x >= C_X+C_SIZE*4+C_SPACE_X*4) && (x <= C_X+C_SIZE*5+C_SPACE_X*4) ) { guiColourID[changedColourID] = 9; drawLightControl(); return; }
	}
	
}

void LED_TFT::handleTouchFileBrowser() {   
    //If back button is pressed 0, 0, 32, 32
	if ( (x >= 0) && (x <= 32) &&  (y >= 0) && (y <= 32)) {
		drawLightControl(); //Function to control the LEDs
		return;
    }
	//Open parent folder 
    else if(y >= 45 && (y < 65) && (x <= 270)) {
		browser.openParentDir();
		drawFileList();
		return;
    }
	//Filelist items 
	for(uint8_t f=0; f<MAX_LIST_ITEMS; f++) {
		if ((y >= FILELIST_Y_TOP + FILELIST_LINE_SIZE*f) && (y <= FILELIST_Y_TOP + FILELIST_LINE_SIZE*(f+1)) && (x <= SCREEN_RES_X-32)  && numFiles>=(f+1) ) {
			fileClick(f);
			return;
		}
	}
		
	//Scroll Up button pressed
    if(x >= SCREEN_RES_X-33 && x <= SCREEN_RES_X-1 && y >= 36 && y <= 68) { //Scroll down button pressed
      if(pageIndex > 0) {
        pageIndex--;
        drawFileList();
		return; 
      }
    }
	//Scroll down button
    else if(x >= SCREEN_RES_X-33 && x <= SCREEN_RES_X-1 && y >= SCREEN_RES_Y-33 && y <= SCREEN_RES_Y-1) { //Scroll down button pressed
      if((browser.getNumOfFiles() - pageIndex*MAX_LIST_ITEMS) > MAX_LIST_ITEMS) {
        pageIndex++;
        drawFileList();
		return;
      }
    }
	else if(x >= 5 && x <= SCREEN_RES_X-64 && y >= SCREEN_RES_Y-33 && y <= SCREEN_RES_Y-1) {
		strlcpy(currentSetting,"Random Colours", sizeof(currentSetting));
		lightMode = 1;
		drawLightControl(); //Function to control the LEDs
		return;
	}
}



void LED_TFT::handleTouchHelpMenu() {
	//If back button is pressed 0, 0, 32, 32
	if ( (x >= 0) && (x <= 32) &&  (y >= 0) && (y <= 32) ) {
		drawLightControl(); //Function to control the LEDs
		return;
    }
	
	//Plus minus of high threshold
	else if ( (x >= MIC_X-43) && (x <= MIC_X-43+16) &&  (y >= SCREEN_RES_Y-MIC_Y*3) && (y <= SCREEN_RES_Y-MIC_Y*3+16) ) {
		if(MIC_THRES_HIGH<=0) MIC_THRES_HIGH = 0;
		else MIC_THRES_HIGH--;
		myLCD.printNumI(MIC_THRES_HIGH,CENTER,SCREEN_RES_Y-MIC_Y*3,3);
	}
	else if ( (x >= MIC_X+30) && (x <= MIC_X+30+16) &&  (y >= SCREEN_RES_Y-MIC_Y*3) && (y <= SCREEN_RES_Y-MIC_Y*3+16) ) {
		if(MIC_THRES_HIGH>=999) MIC_THRES_HIGH = 999;
		else MIC_THRES_HIGH++;
		myLCD.printNumI(MIC_THRES_HIGH,CENTER,SCREEN_RES_Y-MIC_Y*3,3);
	}
	
	//Plus minus of mid threshold
	else if ( (x >= MIC_X-43) && (x <= MIC_X-43+16) &&  (y >= SCREEN_RES_Y-MIC_Y*2) && (y <= SCREEN_RES_Y-MIC_Y*2+16) ) {
		if(MIC_THRES_MID<=0) MIC_THRES_MID = 0;
		else MIC_THRES_MID--;
		myLCD.printNumI(MIC_THRES_MID,CENTER,SCREEN_RES_Y-MIC_Y*2,3);
	}
	else if ( (x >= MIC_X+30) && (x <= MIC_X+30+16) &&  (y >= SCREEN_RES_Y-MIC_Y*2) && (y <= SCREEN_RES_Y-MIC_Y*2+16) ) {
		if(MIC_THRES_MID>=999) MIC_THRES_MID = 999;
		else MIC_THRES_MID++;
		myLCD.printNumI(MIC_THRES_MID,CENTER,SCREEN_RES_Y-MIC_Y*2,3);
	}
	
	//Plus minus of low threshold
	else if ( (x >= MIC_X-43) && (x <= MIC_X-43+16) &&  (y >= SCREEN_RES_Y-MIC_Y) && (y <= SCREEN_RES_Y-MIC_Y+16) ) {
		if(MIC_THRES_LOW<=0) MIC_THRES_LOW = 0;
		else MIC_THRES_LOW--;
		myLCD.printNumI(MIC_THRES_LOW,CENTER,SCREEN_RES_Y-MIC_Y,3);
	}
	else if ( (x >= MIC_X+30) && (x <= MIC_X+30+16) &&  (y >= SCREEN_RES_Y-MIC_Y) && (y <= SCREEN_RES_Y-MIC_Y+16) ) {
		if(MIC_THRES_LOW>=999) MIC_THRES_LOW = 999;
		else MIC_THRES_LOW++;
		myLCD.printNumI(MIC_THRES_LOW,CENTER,SCREEN_RES_Y-MIC_Y,3);
	}
}

//=========== PUBLIC FUNCTIONS CALLED BY MAIN PROGRAMM ===========
void LED_TFT::setupLCD() {
	// Initial touchscreen
	myLCD.InitLCD();
	myLCD.clrScr();
	
	myTouch.begin(WAKE, INTRPT);
	
	pinMode(8, OUTPUT);  //backlight 
	digitalWrite(8, HIGH);//on
  
	
	//Get some default settings; TODO: Check if that works!
	strlcpy(currentSetting,"Random Colours", sizeof(currentSetting));
	lightMode = 1;

	//Standard page
	drawLightControl();  // Draws the Home Screen
	//Serial.println("InitLCD");
}

void LED_TFT::updateDisplay() {
	
	//if(digitalRead(INTRPT) == HIGH) {
		int NBFinger = myTouch.dataread();
		xb = myTouch.readFingerX(0);
		yb = myTouch.readFingerY(0);
		if(NBFinger>0 && (xb!=x || yb!=y) ) {
			x=xb;
			y=yb;
			//x = myTouch.readFingerX(0); // X coordinate where the screen has been pressed
			//y = myTouch.readFingerY(0); // Y coordinates where the screen has been pressed
			
			Serial.print(x);
			Serial.print(" ");
			Serial.println(y);
			// Light Control Screen
			if (currentPage == 0) {
				handleTouchLedSettings();
				return;
			}
			
			// File explorer Screen
			else if (currentPage == 1) {
				//Handle touchscreen of filebrowser
				handleTouchFileBrowser();
				return;
			}
			
			// Colourpicker
			else if (currentPage == 2) {
				//Handle touchscreen of filebrowser
				handleTouchColourPicker();
				return;
			}
			
			// Help menu
			else if (currentPage == 3) {
				//Handle touchscreen of filebrowser
				handleTouchHelpMenu();
				return;
			}
		}
	//}
}


//============ COLOURPICKER FUNCTIONS ===========
uint16_t LED_TFT::getScreenColour(uint8_t colour){
	switch(colour) {
		case 0:
			return VGA_BLACK;
		case 1:
			return VGA_WHITE;
		case 2:
			return VGA_LIME;
		case 3:
			return VGA_RED;
		case 4:
			return VGA_BLUE;
		case 5:
			return VGA_YELLOW;
		case 6:
			return VGA_CYAN;
		case 7:
			return VGA_MAGENTA;
		case 8:
			return VGA_ORANGE;
		case 9:
			return VGA_LIGHTBLUE;
		default:
			return VGA_BLACK;
	}
}
