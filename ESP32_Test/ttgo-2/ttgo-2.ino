#include <TFT_eSPI.h>        //LCD函式庫
#include "Button2.h";

TFT_eSPI tft = TFT_eSPI(); 

#define BUTTON_A_PIN  0  //按鍵A，PIN 0
#define BUTTON_B_PIN  35 //按鍵B，PIN 35
Button2 buttonA = Button2(BUTTON_A_PIN);
Button2 buttonB = Button2(BUTTON_B_PIN);

#include "img1.h"   //載入兩個圖檔，已轉C Array
#include "img2.h"

void setup()
{
  tft.begin();               // 初始化LCD
  tft.setRotation(1);  // landscape
  tft.fillScreen(TFT_BLACK); // 用全黑清除螢幕

  //顯示文字
  
  tft.setFreeFont(&FreeSerifBold24pt7b);  //設定字型，請參考Adafruit GFX函式庫的資料
                                          //https://learn.adafruit.com/adafruit-gfx-graphics-library/using-fonts
  tft.setCursor(30, 60);
  tft.setTextColor(TFT_WHITE);
  tft.setTextSize(1);
  tft.printf("JMaker");
  tft.setFreeFont(&FreeSerifBold12pt7b);
  tft.setCursor(30, 95);
  tft.setTextColor(TFT_YELLOW);
  tft.setTextSize(1);
  tft.printf("jmaker.com.tw");
  delay(2500);

  tft.setSwapBytes(true);
  tft.pushImage(0, 0, 240, 135, img1);  //顯示圖檔img1

  buttonA.setPressedHandler(press);   //建立A按鍵按下Pressed的事件
  buttonA.setReleasedHandler(release);//建立A按鍵放開Released的事件
  buttonB.setPressedHandler(press);   //建立B按鍵按下Pressed的事件
  buttonB.setReleasedHandler(release);//建立B按鍵放開Released的事件
}

void loop()
{
  buttonA.loop();  //重複按鍵的觸發設定
  buttonB.loop();
}

void press(Button2& btn) {
    if (btn == buttonA) {   //按下A按鍵，更換圖片
      tft.pushImage(0, 0, 240, 135, img2);
    } else if (btn == buttonB) {  //按下B按鍵，顯示文字
      tft.fillScreen(TFT_BLACK); // 用全黑清除螢幕
      tft.setFreeFont(&FreeSerifBold24pt7b);
      tft.setCursor(30, 60);
      tft.setTextColor(TFT_WHITE);
      tft.setTextSize(1);
      tft.printf("JMaker");
      tft.setFreeFont(&FreeSerifBold12pt7b);
      tft.setCursor(30, 95);
      tft.setTextColor(TFT_YELLOW);
      tft.setTextSize(1);
      tft.printf("jmaker.com.tw");
    }
}

void release(Button2& btn) {
    if (btn == buttonA) {          //放開按鍵A，恢復顯示圖片
      tft.pushImage(0, 0, 240, 135, img1);
    } else if (btn == buttonB) {   //放開按鍵A，恢復顯示圖片
      tft.pushImage(0, 0, 240, 135, img1);
    }
}
