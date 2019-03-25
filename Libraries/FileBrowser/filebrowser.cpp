/*
	FileBrowser.cpp
*/

#include "filebrowser.h"

#define error(msg)  Serial.print(F("Error: ")); Serial.println(F(msg))
#define info(msg)  Serial.print(F("Info: ")); Serial.println(F(msg))

//======= EXTERNAL VARS ============
extern uint8_t pageIndex;
extern char currentSetting[STRING_LENGTH];
extern uint8_t numFiles;
extern uint8_t numSteps; //Number of the total Steps (Depends on the scene)
extern uint8_t scene[MAX_LIGHT_STEPS][NUM_MUGS];  //Array storing the light scene

//======= CONSTRUCTOR ============
FileBrowser::FileBrowser(const uint8_t sd_Pin){
	SD_CHIP_SELECT = sd_Pin;	// SD card chip select pin.
	strlcpy(CURRENT_PATH, "/", sizeof(CURRENT_PATH));	//Start in Root directory
	CURRENT_INDEX = 0;
	FILE_COUNT = 0;
	SHOW_HIDDEN_FILES = false;
	
	// Initialize the SD card at SPI_FULL_SPEED. Use SPI_HALF_SPEED to avoid bus errors with breadboards.
	if (!SD.begin(SD_CHIP_SELECT, SPI_SPEED)) {
		error("Init SD card failed!");
	}
	
	//Load file list of root directory
	if (!loadFileList()) {
		error("Loading File list failed");
	}
}

//======= PRIVATE FUNCTIONS ============
bool FileBrowser::loadFileList() {
	SD.vwd()->rewind();	//Go to first element in CWD
	//Initialize entries Counter
	int tmpFileCount = 0;
  
	//Go through all files and subDirectories
	while (FILE.openNext(SD.vwd(), O_READ)) {
	//Only not hidden files if flag is not set
	if (!FILE.isHidden() || (FILE.isHidden() && SHOW_HIDDEN_FILES)) {
		FILE.getName(CURRENT_FILE_LIST[tmpFileCount],STRING_LENGTH);    //Write filename in list
		if (FILE.isDir()) {
			// Indicate a directory.
			strcat(CURRENT_FILE_LIST[tmpFileCount], "/");
		}
		CURRENT_INDEX_LIST[tmpFileCount] = FILE.dirIndex();  //Write index in list
		tmpFileCount++;
	}
	
	FILE.close();
  }
  
  //Set number of Entries
  FILE_COUNT = tmpFileCount;
  return true;
}

//======= PUBLIC FUNCTIONS ============
bool FileBrowser::changeDir(char* FolderName) {
	//Save current Directory in string (if changing fails)
	String path = String(CURRENT_PATH);
	
	//If opening path from root folder
	if(FolderName[0] == '/') {
		strlcpy(CURRENT_PATH, FolderName, sizeof(CURRENT_PATH));
		if(String(FolderName).length()>1)	strcat(CURRENT_PATH, "/");	//Add slash if not root dir
		
		//Try to Change directory
		if (!SD.chdir(FolderName,true)) {
			error("Open directory failed!");
			path.toCharArray(CURRENT_PATH, sizeof(CURRENT_PATH));	//Reset path
			return false;			
		}
	}
	//Opening from parent folder ('../<Path>')
	else if(String(FolderName).substring(0,3) == "../") {
		//Try to Open Parent folder
		if(FileBrowser::openParentDir()) {
			path = String(FolderName).substring(3);	//Remove ../ from path
			
			//If path behind ../ call function
			if(path.length()>0) {
				char tmp2[2*STRING_LENGTH];
				path.toCharArray(tmp2, sizeof(tmp2));	//Set path to char array
				FileBrowser::changeDir(tmp2);	//Recursive changeDir
			}
		} else {
			error("Open directory failed!");
			path.toCharArray(CURRENT_PATH, sizeof(CURRENT_PATH));	//Reset path
			return false;
		}
	}
	else {
		strcat(CURRENT_PATH, FolderName);
		strcat(CURRENT_PATH, "/");
		
		//Try to Change directory
		if (!SD.chdir(FolderName,true)) {
			error("Open directory failed!");
			path.toCharArray(CURRENT_PATH, sizeof(CURRENT_PATH));	//Reset path if changing directory failed
			return false;
		}
	}
	
	//Clear list entries
	for (int i=0; i<LIST_LENGTH; i++) {
		strlcpy(CURRENT_FILE_LIST[i], "", sizeof(CURRENT_FILE_LIST[i]));
	}
	 
	//Update List of current Files and SubDirs
	if (!loadFileList()) {
		error("Loading File list failed");
	}
	
	return true; 
}

bool FileBrowser::openParentDir() {
	//convert path in string for analysis
	String path = String(CURRENT_PATH);
	uint8_t slash = path.lastIndexOf("/", path.length() - 2);	//Find second last slash
    String temp;
    if (slash == 0) 
		temp = "/";	//If first char then goto root folder
    else
		temp = path.substring(0, slash+1); //Otherwise set parent path
	
	//Copy new path from string to variable
	temp.toCharArray(CURRENT_PATH, sizeof(CURRENT_PATH));
    if (!SD.chdir(CURRENT_PATH)) {	//Try opening path
		error("Opening parent folder failed");
		path.toCharArray(CURRENT_PATH, sizeof(CURRENT_PATH));	//Reset path
		return false;
	}
	
	//Update List of current Files and SubDirs
	if (!loadFileList()) {
		error("Loading File list failed");
	}
	pageIndex = 0;
	
	return true;
}

//Accessor functions
char* FileBrowser::getCurrentPath() {
	return CURRENT_PATH;
}

uint8_t FileBrowser::getNumOfFiles()const {
	return FILE_COUNT;
}


bool FileBrowser::extractScene(uint8_t (&refScene)[MAX_LIGHT_STEPS][NUM_MUGS], uint8_t &refNumSteps, char (&refCurrentSetting)[STRING_LENGTH]) {
	char stepBuf[NUM_MUGS+1];
	uint8_t p_numSteps = 0;
	bool firstLine = true;
	//Serial.println("refScene:");
	while (FILE.fgets(stepBuf, sizeof(stepBuf)) > 0) {
		//First Line contains Scene name -> Extract to variable
		if(firstLine) {
			String strSetting = String(stepBuf);
			strSetting.remove(strSetting.length() - 1);
			strlcpy(refCurrentSetting,strSetting.c_str(), sizeof(refCurrentSetting));
			firstLine = false;
		}
		else {
			for(int i=0; i<NUM_MUGS; i++) {
				if (isdigit(stepBuf[i])) {
					refScene[p_numSteps][i] = stepBuf[i] - '0';
				}
				else if(stepBuf[i] >= 'A' && stepBuf[i] <= 'F') {	//Can be used if more colours are introduced
					refScene[p_numSteps][i] = stepBuf[i] - '7';
				}
				else {
					refScene[p_numSteps][i] = 0; //Default is black
				}
			}
			 
			p_numSteps++;
			//If the maximum number of light steps is reached stop
			if(p_numSteps == MAX_LIGHT_STEPS) {
				info("Too many Items in List");
				FILE.close();
				refNumSteps = p_numSteps;
				return false;
			}
		}
	}
	refNumSteps = p_numSteps;
	
	return true;
}

//Function executed when a file or directory is clicked on 
uint8_t FileBrowser::onClick(uint8_t fileIndex, uint8_t (&refScene)[MAX_LIGHT_STEPS][NUM_MUGS], uint8_t &refNumSteps, char (&refCurrentSetting)[STRING_LENGTH]) {
	//Open clicked file/directory
	if(!FILE.open(CURRENT_FILE_LIST[fileIndex], O_READ)) {
		error("Opening file failed");
		return 0;
	}
	
	//If clicked on dir call change directory function
	if(FILE.isDir()) {
		FILE.close();
		if(!changeDir(CURRENT_FILE_LIST[fileIndex])) {
			error("Opening folder failed");
			return 0;
		}
		pageIndex = 0;
		return 2;
	}
	//If clicked on a file call extractScene function
	else if (FILE.isFile()) {
		bool errorFlag = extractScene(refScene,refNumSteps,refCurrentSetting);
		FILE.close();
		return errorFlag;
	}
	else {
		FILE.close();
		return 0;
	}
}

