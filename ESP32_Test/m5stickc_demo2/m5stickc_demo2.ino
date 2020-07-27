//m5stickC範例程式，接DHT11
#include <M5StickC.h>        // Hardware-specific library

#include "img1.h"
#include "img2.h"



/*
#define FILTER_N 12

float Filter(char DIR)   // DIR may be 'x','y','z' 
{
    float accX, accY, accZ;

    // 平均滤波法
    float filterSum = 0.0;
    
    for(int i = 0; i < FILTER_N; i++) 
    {
        M5.IMU.getAccelData(&accX, &accY, &accZ);
        if(DIR == 'x')
          filterSum += accX ;
        else if(DIR == 'y')
          filterSum += accY ;
        else
          filterSum += accZ ;

        delay(5);
    }
    
    return (float)(filterSum / FILTER_N);
}
*/

#define FILTER_N_3AIX 36

void Filter_3aix(float * filterSum_X, float * filterSum_Y, float * filterSum_Z)  
{
    float accX, accY, accZ;

    // 平均滤波法
    
    for(int i = 0; i < FILTER_N_3AIX; i++) 
    {
        M5.IMU.getAccelData(&accX, &accY, &accZ);

        *filterSum_X = *filterSum_X + accX ;
        *filterSum_Y = *filterSum_Y + accY ;
        *filterSum_Z = *filterSum_Z + accZ ;

        delay(5);
    }

    *filterSum_X = *filterSum_X * 10 / FILTER_N_3AIX ;
    *filterSum_Y = *filterSum_Y * 10 / FILTER_N_3AIX ;
    *filterSum_Z = *filterSum_Z * 10 / FILTER_N_3AIX ;
    
}


void setup() 
{
  M5.begin();               // Initialise the display

  int ret = M5.IMU.Init(); //return 0 is ok, return -1 is unknow
  Serial.printf("IMU init return value = %d\n",ret);  
  
  pinMode(M5_BUTTON_HOME, INPUT);  //設定ButtonA
  pinMode(M5_BUTTON_RST, INPUT);   //設定ButtonB
  pinMode(M5_LED, OUTPUT);         //設定內建的LED
  
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
  /*
  float accX, accY, accZ;
  M5.IMU.getAccelData(&accX, &accY, &accZ);
  M5.Lcd.fillScreen(BLACK); 
  M5.Lcd.setCursor(0, 20);
  accX*=10 ;
  accY*=10 ;
  accZ*=10 ;
  M5.Lcd.printf("X:%5.2f\nY:%5.2f\nZ:%5.2f ", accX, accY, accZ);
  Serial.printf("X:%5.2f\nY:%5.2f\nZ:%5.2f ", accX, accY, accZ);
  */

//~~~~~~~~~~~STEP COUNTER~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//#define STEP_THD 0.05
#define STEP_THD 0.1
   
   static float oldAccx, oldAccy, oldAccz, oldDot;
   static int stepNumber = 0;

   float accx ;//= Filter('x');  // 读取滤波后的加速度 x 分量
   float accy ;//= Filter('y');
   float accz ;//= Filter('z');
   Filter_3aix(&accx, &accy, &accz) ;
   float dot = (oldAccx * accx)+(oldAccy * accy)+(oldAccz * accz);

   float oldAcc = abs(sqrt(oldAccx * oldAccx + oldAccy * oldAccy + oldAccz * oldAccz));
   float newAcc = abs(sqrt(accx * accx + accy * accy + accz * accz));
   dot /= (oldAcc * newAcc);  // 计算加速度变化程度

   Serial.printf("dot:%5.3f\noldDot:%5.3f\n", dot, oldDot);

   if(abs(dot - oldDot) >= STEP_THD) 
   {
        // 变化程度超过阈值，则判定步数增加
        // 并打印
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
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 
  //當使用者按下ButtonA，顯示img2圖片，按下時LED亮起，放開LED就滅了
  if(digitalRead(M5_BUTTON_HOME) == LOW){
    M5.Lcd.pushImage(0, 0, 160, 80, img2);
    digitalWrite(M5_LED, LOW);

    stepNumber = 0 ;
  }else{
    digitalWrite(M5_LED, HIGH);
  }

  //讓ButtonB擁有正常的Reset功能
  if(digitalRead(M5_BUTTON_RST) == LOW){
    esp_restart();
  }

  delay(200);
 
}
