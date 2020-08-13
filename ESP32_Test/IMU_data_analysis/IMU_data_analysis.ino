//
#include "config.h"
#include "step_counting.h"
#include <Ticker.h>

#include "img1.h"
#include "img2.h"

#ifdef TARGET_M5STACK_GARY
  Ticker BeepTimer ;

  void beep_callback(int arg)
  {
    M5.Speaker.tone(NOTE_DH2, 200); //frequency 3000, with a duration of 200ms
    Serial.println("Beep!");
  }

#endif

void setup() 
{
  M5.begin();               // Initialise the display

  #ifdef TARGET_M5STACK_GARY
    /*
    Power chip connected to gpio21, gpio22, I2C device
    Set battery charging voltage and current
    If used battery, please call this function in your project
    */  
    M5.Power.begin();

  #endif

  int ret = M5.IMU.Init(); //return 0 is ok, return -1 is unknow
  Serial.printf("IMU init return value = %d\n",ret);  

  #ifdef TARGET_M5STICK_C
    pinMode(M5_BUTTON_HOME, INPUT);  //設定ButtonA
    pinMode(M5_BUTTON_RST, INPUT);   //設定ButtonB
    pinMode(M5_LED, OUTPUT);         //設定內建的LED
  #endif
  
  M5.Lcd.fillScreen(TFT_BLACK); // 螢幕全黑

  M5.Lcd.setSwapBytes(true);  //要加這行，不然顏色會不正確
  M5.Lcd.setRotation(1);      //M5StickC預設LCD是直的，所以如果我們要讓它橫的顯示，就要轉90度。
  M5.Lcd.pushImage(0, 0, 160, 80, img1);  //顯示img1的圖片，x,y,寬,高,圖檔陣列名
  delay(1000);
  //M5.Lcd.fillScreen(BLACK);
  
  //顯示圖片，如果沒有清除畫面，文字就會壓在之前的圖片上
  //M5.Lcd.setFreeFont(&FreeSerifBold24pt7b);  //設定字型，請參考Adafruit GFX函式庫的資料
                                             //https://learn.adafruit.com/adafruit-gfx-graphics-library/using-fonts
  M5.Lcd.setFreeFont(&FreeSerifBold18pt7b);   
                                            
  M5.Lcd.setCursor(0, 40);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(1);
  M5.Lcd.printf("JMaker");
  M5.Lcd.setFreeFont(&FreeSerifBold12pt7b);
  M5.Lcd.setCursor(2, 65);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(1);
  M5.Lcd.printf("jmaker.com.tw");
  delay(1500);
  M5.Lcd.fillScreen(TFT_BLACK);

  M5.Lcd.setCursor(2, 60);
  M5.Lcd.printf("GOGOGO");

}



void loop() 
{
 enum
 {
  MODE_SETP_COUNTING =0,
  MODE_DATA_COLLECTION
 } ;
 
 static int mode = MODE_SETP_COUNTING ;
 int step_number = 0 ;

 #if _CALCULATE_SAMPLING_TIME_ == 1
 static unsigned long counter = 0 ;
 unsigned long t1,t2,t3 ;
 #endif


 if(mode == MODE_SETP_COUNTING)
 {
     static int color[] = {BLACK, BLUE, GREEN, RED} ;

     #if _CALCULATE_SAMPLING_TIME_ == 1
       t1 = millis();
     #endif
    
     step_number = sampling_new_step() ;
     if(step_number != 0)
     {
       int index = step_number % 4 ;
       Serial.println(step_number);
       M5.Lcd.fillScreen(color[index]); 
       M5.Lcd.setCursor(0, 20);
       M5.Lcd.printf("step: %5d", step_number);
     }
    
     #if _CALCULATE_SAMPLING_TIME_ == 1
        t2 = millis();
        Serial.printf("loop #%5d: %5d ms - sampling once\n", counter,t2-t1) ;
     #endif
 }
 else
 {
    // mode == MODE_DATA_COLLECTION
    sample_and_save_one_record() ;  // save 1 record
 }


 //---------------KEY event handling -------------------------
 #ifdef TARGET_M5STACK_GARY
    if(M5.BtnA.wasPressed()) 
    {      
      #if _DEBUG_LOG_ == 1
         Serial.printf("Key-A pressed - Step counting mode") ;
      #endif
      M5.Lcd.fillScreen(BLACK); 
      M5.Lcd.setCursor(0, 20);      
      M5.Lcd.println("[Step-counting Mode]") ;
      mode = MODE_SETP_COUNTING ;
      reset_step_number() ;
      stop_collection() ;
    }
    else if(M5.BtnC.wasPressed())  // enter data collection mode
    {
      #if _DEBUG_LOG_ == 1
         Serial.printf("Key-C presseGd - data collection mode") ;
      #endif
      mode = MODE_DATA_COLLECTION ;
      start_collection() ;
 
      M5.Lcd.fillScreen(BLACK);
      M5.Lcd.setCursor(2, 60);
      M5.Lcd.println("[Data collection Mode]\n") ;
    }
    else if(M5.BtnB.wasPressed())  // start/stop 節拍器
    {
      static int beep_status = 0 ;
      if(beep_status == 0)
      {
        BeepTimer.attach_ms(500, beep_callback,0) ;
        beep_status = 1 ;
      }
      else
      {
        BeepTimer.detach() ;
        beep_status = 0 ;
      }
    }
  
 #endif

 #ifdef TARGET_M5STICK_C
    //當使用者按下ButtonA，顯示img2圖片，按下時LED亮起，放開LED就滅了
    if(digitalRead(M5_BUTTON_HOME) == LOW)
    {
      M5.Lcd.pushImage(0, 0, 160, 80, img2);
      digitalWrite(M5_LED, LOW);
  
      reset_step_number() ;
    }
    else
    {
      digitalWrite(M5_LED, HIGH);
    }
  
    //讓ButtonB擁有正常的Reset功能
    if(digitalRead(M5_BUTTON_RST) == LOW)
    {
      esp_restart();
    }
 #endif


 if(mode == MODE_SETP_COUNTING)
 {
    delay(50);   // 必須使每秒計步4~5次  (M5-Gray: 每個loop 260ms)
    
    #if _CALCULATE_SAMPLING_TIME_ == 1
        t3 = millis();
        Serial.printf("loop #%5d: %5d ms - loop once - %5d, %5d, %5d\n", counter++,t3-t1,t1,t2,t3) ;
    #endif
 }  
 
 M5.update();  // key status update
 
}
