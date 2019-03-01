#ifndef FileBrowser_h
#define FileBrowser_h

#include <SPI.h>
#include <SdFat.h>

#define STRING_LENGTH	32
#define LIST_LENGTH		20
#define SPI_SPEED		SPI_FULL_SPEED
#define MAX_LIGHT_STEPS 50
#define NUM_MUGS 		41


class FileBrowser {
  private:
	uint8_t SD_CHIP_SELECT;	// SD card chip select pin.
	char CURRENT_PATH[2*STRING_LENGTH];	//path of CWD
	uint8_t CURRENT_INDEX;	//Index of current Element
	uint8_t FILE_COUNT;	//Number of elements in CWD
	bool SHOW_HIDDEN_FILES;
	
	SdFat SD;// File system object.
	SdFile FILE;// Use for file creation in folders.
	
	//======= PRIVATE FUNCTIONS ============
	bool loadFileList();
	
//----------------------------------------------
  public:
	//Constructor
	FileBrowser(const uint8_t sd_Pin);
	
	//Public variables
	//FileList
	char CURRENT_FILE_LIST[LIST_LENGTH][STRING_LENGTH];
	uint8_t CURRENT_INDEX_LIST[LIST_LENGTH];
	
	//Accessors
	char* getCurrentPath();
	uint8_t getNumOfFiles() const;
	bool changeDir(char* FolderName);
	bool openParentDir();
	bool extractScene(uint8_t (&refScene)[MAX_LIGHT_STEPS][NUM_MUGS], uint8_t &refNumSteps, char (&refCurrentSetting)[STRING_LENGTH]);
	//uint8_t onClick(uint8_t fileIndex);
	uint8_t onClick(uint8_t fileIndex, uint8_t (&refScene)[MAX_LIGHT_STEPS][NUM_MUGS], uint8_t &refNumSteps, char (&refCurrentSetting)[STRING_LENGTH]);
	
};

#endif
