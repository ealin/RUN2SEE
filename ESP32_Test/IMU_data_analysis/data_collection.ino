#include "config.h"
#include "step_counting.h"
#include <EEPROM.h>

static int start_falg = 0 ;
static File File_handle ;


#define DATA_FILE_NAME "/acc_gyro_time"


// return 0 if open file in SD-card error
//
int start_collection(void) // open the file
{
 if(start_falg == 1)
  return 1;
  
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

 //read serial number of data file from EEPROM
 //
 unsigned char eeprom_data[4] ;
 unsigned int * serial_number = (unsigned int *)eeprom_data ;
 char data_file_name[32] ;
 
 if(!EEPROM.begin(64))
 {
    Serial.println("failed to initialise EEPROM");   
    (*serial_number) = 5410 ;
 }
 else
 {
     Serial.println("Sucess to initialise EEPROM");   
     eeprom_data[0] = EEPROM.read(0) ;
     eeprom_data[1] = EEPROM.read(1) ;
     eeprom_data[2] = EEPROM.read(2) ;
     eeprom_data[3] = EEPROM.read(3) ;
    
     (*serial_number)++ ;
     EEPROM.write(0, eeprom_data[0]) ;
     EEPROM.write(1, eeprom_data[1]) ;
     EEPROM.write(2, eeprom_data[2]) ;
     EEPROM.write(3, eeprom_data[3]) ;
     EEPROM.commit() ;
 }

 sprintf(data_file_name, "%s_%d.csv",DATA_FILE_NAME,(*serial_number) ) ;
 Serial.printf("data file name is %s\n",data_file_name) ;
 
 // open DATA_FILE in FILE_APPEND mode
 File_handle = SD.open(data_file_name, FILE_APPEND);
 if(!File_handle){
        Serial.println("Failed to open file for appending");
        return 0;
 }    

 start_falg =1 ;  
 Serial.println("start data collection - after 10sec");
 delay(5*1000) ;  // unit: ms
 Serial.println("start data collection - GO!!!");
 
 return 1 ;
}


void stop_collection(void)  // close the file
{
 if(start_falg == 0)
  return ; 
  
 File_handle.close(); 
 start_falg = 0 ; 
 Serial.println("stop data collection");

}


void sample_and_save_one_record(void)
{
 if(start_falg == 0)
    return ;

 float accX = 0.0F;
 float accY = 0.0F;
 float accZ = 0.0F;

 float gyroX = 0.0F;
 float gyroY = 0.0F;
 float gyroZ = 0.0F;

 char record_str[100] ;
 static int record_no = 0 ;

 static unsigned long old_t = 0 ;
 unsigned long current_t ;
 
 M5.IMU.getGyroData(&gyroX,&gyroY,&gyroZ);
 M5.IMU.getAccelData(&accX,&accY,&accZ);

 sprintf(record_str,"%5d, %d,%.5f,%.5f,%.5f,%.5f,%.5f,%.5f\n",record_no, millis(),accX,accY,accZ,gyroX,gyroY,gyroZ) ;

 if(File_handle.print(record_str))
 {
    current_t = millis() ;
    if(record_no != 0)
      Serial.printf("Record #%5d is appended, loop time = %5d ms\n",record_no ,current_t-old_t);

    record_no++ ;
    old_t = current_t ;    
 } 
 else 
 {
    Serial.println("Append failed");
 } 
    
}
