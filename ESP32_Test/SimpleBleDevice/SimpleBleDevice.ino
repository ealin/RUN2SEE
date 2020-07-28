// Copyright 2015-2016 Espressif Systems (Shanghai) PTE LTD
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Sketch shows how to use SimpleBLE to advertise the name of the device and change it on the press of a button
// Useful if you want to advertise some sort of message
// Button is attached between GPIO 0 and GND, and the device name changes each time the button is pressed

#include <M5StickC.h>        // Hardware-specific library
#include "SimpleBLE.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
XXX
#endif

SimpleBLE ble;

void onButton(){
    String out = "BLE32 name: ";
    out += String(millis() / 1000);
    Serial.println(out);
    ble.begin(out);
}

void setup() {

  pinMode(M5_BUTTON_HOME, INPUT);  //設定ButtonA
  pinMode(M5_BUTTON_RST, INPUT);   //設定ButtonB
  pinMode(M5_LED, OUTPUT);         //設定內建的LED
   
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    //pinMode(0, INPUT_PULLUP);
    Serial.print("ESP32 SDK: ");
    Serial.println(ESP.getSdkVersion());
    ble.begin("ESP32 SimpleBLE- Ealin");
    Serial.println("Press the button to change the device's name");
}

void loop() {
    //static uint8_t lastPinState = 1;
    //uint8_t pinState = digitalRead(0);
    //if(!pinState && lastPinState)
   /*
    if(digitalRead(M5_BUTTON_HOME) == LOW)    
    {
        onButton();
    }
    */
    //lastPinState = pinState;
    
    while(Serial.available()) 
      Serial.write(Serial.read());
}
