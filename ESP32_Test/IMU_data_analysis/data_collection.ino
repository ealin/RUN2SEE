#include "config.h"
#include "step_counting.h"

static int start_falg = 0 ;
static File File_handle ;


// return 0 if open file in SD-card error
//
int start_collection(void) // open the file
{
 static int SD_init_flag = 0 ;
 if(SD_init_flag == 0)
 {   
    if(!SD.begin()){
        Serial.println("Card Mount Failed");
        return 0;
    }
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return 0;
    }

    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
        return 0 ;
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);
    Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
    Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));  
    SD_init_flag = 1 ;
 }   

 // open a new file
 start_falg =1 ;  
 
 return 1 ;
}


void stop_collection(void)  // close the file
{
 start_falg = 0 ; 
}


void sample_and_save_one_record(void)
{
  if(start_falg == 0)
    return ;
    
}
