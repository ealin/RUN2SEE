//m5stickC範例程式，接DHT11
#include <M5StickC.h>        // Hardware-specific library

#include "img1.h"
#include "img2.h"

void setup() {
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
  M5.Lcd.setFreeFont(&FreeSerifBold24pt7b);  //設定字型，請參考Adafruit GFX函式庫的資料
                                             //https://learn.adafruit.com/adafruit-gfx-graphics-library/using-fonts
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

/*
  //顯示DHT的溫濕度到LCD上
  float h = dht.readHumidity();   //取得濕度
  float t = dht.readTemperature();  //取得溫度C
  M5.Lcd.pushImage(0, 0, 160, 80, img1);
  M5.Lcd.setCursor(2, 30);
  M5.Lcd.printf("RH    : %g %%", h);
  M5.Lcd.setCursor(2, 60);
  M5.Lcd.printf("Temp: %g *C", t);
  */
}

void loop() {
  float accX, accY, accZ;
  M5.IMU.getAccelData(&accX, &accY, &accZ);
  M5.Lcd.fillScreen(BLACK); 
  M5.Lcd.setCursor(0, 45);
  M5.Lcd.printf("X:%5.2f\nY:%5.2f\nZ:%5.2f ", accX, accY, accZ);
  Serial.printf("X:%5.2f\nY:%5.2f\nZ:%5.2f ", accX, accY, accZ);
  
  //當使用者按下ButtonA，顯示img2圖片，按下時LED亮起，放開LED就滅了
  if(digitalRead(M5_BUTTON_HOME) == LOW){
    M5.Lcd.pushImage(0, 0, 160, 80, img2);
    digitalWrite(M5_LED, LOW);
  }else{
    digitalWrite(M5_LED, HIGH);
  }

  //讓ButtonB擁有正常的Reset功能
  if(digitalRead(M5_BUTTON_RST) == LOW){
    esp_restart();
  }

  delay(500);
 
}
