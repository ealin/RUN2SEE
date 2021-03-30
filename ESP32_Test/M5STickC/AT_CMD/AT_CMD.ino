

#include <M5StickC.h>

#define RX_PIN      33
#define TX_PIN      32


#define MAX_BUF_SIZE 2048
#define MAX_RETRY_COUNTER 20
char * AT_CMD_response_buffer[MAX_BUF_SIZE] ;


// メッセージ出力
void showMessage(const char *message) {
    M5.Lcd.fillScreen(WHITE);
    M5.Lcd.setCursor(5, 35);
    M5.Lcd.setTextFont(2);
    M5.Lcd.setTextColor(BLACK);
    M5.Lcd.print(message);
}


// LOWでLED点灯、HIGHでLED消灯
const uint8_t LED_ON = LOW;
const uint8_t LED_OFF = HIGH;
const uint8_t AXP_WAS_PRESSED = 2;

void setup() 
{
    M5.begin();
    M5.IMU.Init();
    
    M5.Lcd.setRotation(1);
    //showMessage("AT-CMD to N10SG");
    showMessage("AT-CMD to N10SG");


    Serial.begin(115200);
    Serial2.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);      //配置串口 2

    // LEDの設定
    pinMode(M5_LED, OUTPUT);
    digitalWrite(M5_LED, LED_OFF);

    // connect to internet
    int byte_read = send_AT_command("AT") ;
    Serial.printf("#1. response bytes number = %d, feedback is %s \n",byte_read,AT_CMD_response_buffer ) ;

    byte_read = send_AT_command("AT+SM=LOCK_FOREVER") ;
    Serial.printf("#2. response bytes number = %d, feedback is %s \n",byte_read,AT_CMD_response_buffer ) ;

    byte_read = send_AT_command("AT+CGDCONT=1,\"IP\",\"internet.iot\"") ;
    Serial.printf("#3. response bytes number = %d, feedback is %s \n",byte_read,AT_CMD_response_buffer ) ;

    byte_read = send_AT_command("AT+COPS=1,2,\"46692\"") ;
    Serial.printf("#4. response bytes number = %d, feedback is %s \n",byte_read,AT_CMD_response_buffer ) ;

    byte_read = send_AT_command("AT+EGACT=1,1,\"\",\"\",\"\"") ;
    Serial.printf("#5. response bytes number = %d, feedback is %s \n",byte_read,AT_CMD_response_buffer ) ;


    // Initalize GPS
    delay(1000) ;
    byte_read = send_AT_command("AT+CMGMODE=1") ;// 设置为AGNSS模式  (資料量約5KB)
    Serial.printf("#5. response bytes number = %d, feedback is %s \n",byte_read,AT_CMD_response_buffer ) ;
    
    byte_read = send_AT_command("AT+CMGNMEA=63") ;// 允许所有NMEA输出
    Serial.printf("#5. response bytes number = %d, feedback is %s \n",byte_read,AT_CMD_response_buffer ) ;

    byte_read = send_AT_command("AT+CMGLOC=1") ;// 定位结果自动上报
    Serial.printf("#5. response bytes number = %d, feedback is %s \n",byte_read,AT_CMD_response_buffer ) ;

    byte_read = send_AT_command("AT+CMGRUN=2") ;// 启动连续跟踪模式   (1: 單次跟蹤模式)
    Serial.printf("#5. response bytes number = %d, feedback is %s \n",byte_read,AT_CMD_response_buffer ) ;

    //delay(1000) ;
    //byte_read = send_AT_command("AT+CMGREAD?") ;// Read GPS data
    //Serial.printf("#5. response bytes number = %d, feedback is %s \n",byte_read,AT_CMD_response_buffer ) ;

    // subscribe MQTT channel for this device
    //

    // config. MQTT server
    //server ID, port, client ID, keepalive time, user name, password, clean是否刪除session (o or 1)
    byte_read = send_AT_command("AT+MQTTCFG=\"broker.emqx.io\",\"1883\",\"75681112\",60,\"\",***,1,0") ;   // MQTT config.
    Serial.printf("AT+MQTTCFG: response bytes number = %d, feedback is %s \n",byte_read,AT_CMD_response_buffer ) ;

    // open MQTT , no need user name & pwd
    byte_read = send_AT_command("AT+MQTTOPEN=0,0,0,0,0,\"\",\"\"") ;   // MQTT config.
    Serial.printf("AT+MQTTOPEN: response bytes number = %d, feedback is %s \n",byte_read,AT_CMD_response_buffer ) ;

    // subscribe a channel
    delay(2000) ;
    byte_read = send_AT_command("AT+MQTTSUB=\"MAI/TEST\",0") ;   // 訂閱主題, MAI/TEST:   topic, qos
    Serial.printf("AT+MQTTSUB: response bytes number = %d, feedback is %s \n",byte_read,AT_CMD_response_buffer ) ;

#
}


// Reference: Arduino Serial Objects
//   https://www.arduino.cc/reference/en/language/functions/communication/serial/

// return : bytes number feedback by device  
//
int send_AT_command(char * cmd)
{
 int bytes_read = 0 ;
 int max_retry_counter = 0 ;

 Serial2.println(cmd) ;
 while( Serial2.available() <= 0 && max_retry_counter <= MAX_RETRY_COUNTER)
 {
    delay(100) ;
    max_retry_counter++ ;
 }
 Serial.printf("command is : %s, max_retry_counter = %d\n", cmd, max_retry_counter) ;
 
 bytes_read = 0 ;
 if(max_retry_counter <= MAX_RETRY_COUNTER) 
 {
    memset((void *)AT_CMD_response_buffer,0, MAX_BUF_SIZE) ;
    bytes_read = Serial2.readBytes((char *)AT_CMD_response_buffer,(size_t)MAX_BUF_SIZE ) ;
 }
    
 AT_CMD_response_buffer[bytes_read] = '\0' ;
    
 return bytes_read ;
}



void loop() 
{
    // 9-aixl data from IMU
    float accX = 0.0F;
    float accY = 0.0F;
    float accZ = 0.0F;   
    float gyroX = 0.0F;
    float gyroY = 0.0F;
    float gyroZ = 0.0F;
    float pitch = 0.0F;
    float roll  = 0.0F;
    float yaw   = 0.0F;
  
    static int counter = 0 ;
    int byte_read = 0 ;
    char message[64] ;

    delay(10);

    
    M5.update();// ボタン(KEY)の状態を更新
    if (M5.BtnA.wasPressed()) 
    {
        // 1.LED
        //
        if(counter++ % 2 == 0)
          digitalWrite(M5_LED, LED_ON);// LED点灯
        else
          digitalWrite(M5_LED, LED_OFF);// LED消灯

        // 1.AT command
        //
        //byte_read = send_AT_command("AT") ;
        //Serial.printf("AT: response bytes number = %d, feedback is %s \n",byte_read,AT_CMD_response_buffer ) ;

        // 2. Internet
        //
        if(counter <= 3)
        {
            byte_read = send_AT_command("AT+PING=8.8.8.8") ;  // PING
            Serial.printf("AT+PING: response bytes number = %d, feedback is %s \n",byte_read,AT_CMD_response_buffer ) ;
        }
        
        // 3.GPS
        //
        //byte_read = send_AT_command("AT+CMGLOC") ;   //  查询定位信息
        //Serial.printf("AT+CMGLOC: response bytes number = %d, feedback is %s \n",byte_read,AT_CMD_response_buffer ) ;

        // 4. IMU
        // check motion or not
        M5.IMU.getGyroData(&gyroX,&gyroY,&gyroZ);
        M5.IMU.getAccelData(&accX,&accY,&accZ);
        M5.IMU.getAhrsData(&pitch,&roll,&yaw);
        Serial.printf("IMU data: %f %f %f %f %f %f %f %f %f\r\n", gyroX,gyroY,gyroZ,accX,accY,accZ,pitch,roll,yaw) ;

        // 5. publish a message
        //
        sprintf(message,"AT+MQTTPUB=\"MAI/TEST\",0,1,0,0,\"[%3d] message published\"", counter) ;
        byte_read = send_AT_command(message) ;  
        Serial.printf("AT+MQTTPUB: response bytes number = %d, feedback is %s \n",byte_read,AT_CMD_response_buffer ) ;

    }

    // 電源ボタンが押されたらリセット   / 按下電源按鈕時重置
    if (M5.Axp.GetBtnPress() == AXP_WAS_PRESSED) {
        esp_restart();
    }

/*

  Serial.printf("S1#%d\r\n", counter++) ;
  Serial2.printf("S2#%d\r\n", counter++) ;

  delay(5000);
*/  

}
