//
#include "config.h"
#include "step_counting.h"

#include "img1.h"
#include "img2.h"



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
 #if _CALCULATE_SAMPLING_TIME_ == 1
   unsigned long t1 = millis();
   static unsigned long counter = 0 ;
 #endif
//~~~~~~~~~~~STEP COUNTER - BEGIN~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

   static float oldAccx, oldAccy, oldAccz, oldDot;
   static int stepNumber = 0;

#ifdef SAMPLING_METHOD_1BY1
   float accx = Filter('x') ;  // 读取滤波后的加速度 x 分量
   float accy = Filter('y') ;
   float accz = Filter('z') ;
#else
   float accx ; // 读取滤波后的加速度 x 分量
   float accy ;
   float accz ;
   Filter_3aix(&accx, &accy, &accz) ;
#endif
   
   float dot = (oldAccx * accx)+(oldAccy * accy)+(oldAccz * accz);

   float oldAcc = abs(sqrt(oldAccx * oldAccx + oldAccy * oldAccy + oldAccz * oldAccz));
   float newAcc = abs(sqrt(accx * accx + accy * accy + accz * accz));
   dot /= (oldAcc * newAcc);  // 计算加速度变化程度

   #if _DEBUG_LOG_ == 1
     Serial.printf("dot:%5.3f\noldDot:%5.3f\n", dot, oldDot);
   #endif

   if(abs(dot - oldDot) >= STEP_THD) 
   {
        // 变化程度超过阈值，则判定步数增加; 并打印
        stepNumber += 1;
        Serial.println(stepNumber);

        M5.Lcd.fillScreen(BLACK); 
        M5.Lcd.setCursor(0, 20);
        M5.Lcd.printf("step: %5d", stepNumber);
        
   }
    
   oldAccx = accx;
   oldAccy = accy;
   oldAccz = accz;
   oldDot = dot;
//~~~~~~~~~~~STEP COUNTER - END~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  #if _CALCULATE_SAMPLING_TIME_ == 1
    unsigned long t2 = millis();
    Serial.printf("loop #%5d: %5d ms - sampling once\n", counter,t2-t1) ;
  #endif

  #ifdef TARGET_M5STACK_GARY
    if(M5.BtnA.wasPressed()) 
    {
      #if _DEBUG_LOG_ == 1
         Serial.printf("Key-A pressed") ;
      #endif
      M5.Lcd.pushImage(0, 0, 160, 80, img2);
      stepNumber = 0 ;
    }
  #endif

   #ifdef TARGET_M5STICK_C
    //當使用者按下ButtonA，顯示img2圖片，按下時LED亮起，放開LED就滅了
    if(digitalRead(M5_BUTTON_HOME) == LOW)
    {
      M5.Lcd.pushImage(0, 0, 160, 80, img2);
      digitalWrite(M5_LED, LOW);
  
      stepNumber = 0 ;
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

  delay(50);
  M5.update();  // key status update

  #if _CALCULATE_SAMPLING_TIME_ == 1
    unsigned long t3 = millis();
    Serial.printf("loop #%5d: %5d ms - loop once - %5d, %5d, %5d\n", counter++,t3-t1,t1,t2,t3) ;
  #endif
  
 
}
