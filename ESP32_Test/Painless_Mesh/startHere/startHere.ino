//************************************************************
// this is a simple example that uses the easyMesh library
//
// 1. blinks led once for every node on the mesh
// 2. blink cycle repeats every BLINK_PERIOD
// 3. sends a silly message to every node on the mesh at a random time between 1 and 5 seconds
// 4. prints anything it receives to Serial.print
//
//
//************************************************************
#include <painlessMesh.h>

#define BOARD_TTGO_TDISPLAY
//#define BOARD_M5STICK_C
//#define BOARD_M5STACK_GARY

#ifdef BOARD_TTGO_TDISPLAY
  #include <TFT_eSPI.h>        //LCD函式庫
  #include <Button2.h>
  
  TFT_eSPI tft = TFT_eSPI(); 
  
  #define BUTTON_A_PIN  0  //按鍵A，PIN 0
  #define BUTTON_B_PIN  35 //按鍵B，PIN 35
  Button2 buttonA = Button2(BUTTON_A_PIN);
  Button2 buttonB = Button2(BUTTON_B_PIN);
#endif

#ifdef BOARD_M5STICK_C
  #include <M5StickC.h>
#endif


// some gpio pin that is connected to an LED...
// on my rig, this is 5, change to the right number of your LED.
#define   LED             2       // GPIO number of connected LED, ON ESP-12 IS GPIO2

#define   BLINK_PERIOD    3000 // milliseconds until cycle repeat
#define   BLINK_DURATION  1000  // milliseconds LED is on for

#define   MESH_SSID       "MAI_2.4GHz"
#define   MESH_PASSWORD   "0227200846"
#define   MESH_PORT       5555

// Prototypes
void sendMessage(); 
void receivedCallback(uint32_t from, String & msg);
void newConnectionCallback(uint32_t nodeId);
void changedConnectionCallback(); 
void nodeTimeAdjustedCallback(int32_t offset); 
void delayReceivedCallback(uint32_t from, int32_t delay);

Scheduler     userScheduler; // to control your personal task
painlessMesh  mesh;

bool calc_delay = false;
SimpleList<uint32_t> nodes;

void sendMessage() ; // Prototype
Task taskSendMessage( TASK_SECOND * 1, TASK_FOREVER, &sendMessage ); // start with a one second interval

// Task to blink the number of nodes
Task blinkNoNodes;
bool onFlag = false;

void setup() 
{
  #ifdef BOARD_M5STICK_C
    M5.begin(); 
    M5.Lcd.setSwapBytes(true);  //要加這行，不然顏色會不正確
    M5.Lcd.setRotation(1);      //M5StickC預設LCD是直的，所以如果我們要讓它橫的顯示，就要轉90度。
  #endif
  
  Serial.begin(115200);

  //pinMode(LED, OUTPUT);
  #ifdef BOARD_TTGO_TDISPLAY
    tft.begin();                // 初始化LCD
    tft.setRotation(1);         // landscape
    tft.fillScreen(TFT_BLACK);  // 用全黑清除螢幕

    tft.setFreeFont(&FreeSerifBold12pt7b);  //設定字型，請參考Adafruit GFX函式庫的資料
                                            //https://learn.adafruit.com/adafruit-gfx-graphics-library/using-fonts
    tft.setCursor(10, 10);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(1);
    tft.printf("MAI Mesh Test");
    delay(2000) ;
                                            
  #endif
  
  mesh.setDebugMsgTypes(ERROR | DEBUG);  // set before init() so that you can see error messages

  mesh.init(MESH_SSID, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.onNodeDelayReceived(&delayReceivedCallback);

  userScheduler.addTask( taskSendMessage );
  taskSendMessage.enable();

  blinkNoNodes.set(BLINK_PERIOD, (mesh.getNodeList().size() + 1) * 2, []() {    // Task: blinkNoNodes   (控制LED閃爍)
      // If on, switch off, else switch on
      if (onFlag)
        onFlag = false;
      else
        onFlag = true;
      blinkNoNodes.delay(BLINK_DURATION);

      if (blinkNoNodes.isLastIteration()) {
        // Finished blinking. Reset task for next run 
        // blink number of nodes (including this node) times
        blinkNoNodes.setIterations((mesh.getNodeList().size() + 1) * 2);
        // Calculate delay based on current mesh time and BLINK_PERIOD
        // This results in blinks between nodes being synced
        blinkNoNodes.enableDelayed(BLINK_PERIOD - 
            (mesh.getNodeTime() % (BLINK_PERIOD*1000))/1000);
      }
  });
  userScheduler.addTask(blinkNoNodes);
  blinkNoNodes.enable();

  randomSeed(5410/*analogRead(A0)*/);
}

void loop() {
  mesh.update();
  
  //digitalWrite(LED, !onFlag);    // <=== turn on/off LED
  #ifdef BOARD_TTGO_TDISPLAY
    if(onFlag)                    // 用LCD替代LED
      tft.fillScreen(TFT_GREEN);  
    else
      tft.fillScreen(TFT_BLUE); 
  #endif

  #ifdef BOARD_M5STICK_C
    if(onFlag)                    // 用LCD替代LED
      M5.Lcd.fillScreen(TFT_GREEN);  
    else
      M5.Lcd.fillScreen(TFT_BLUE); 
  #endif

  
}

void sendMessage() 
{
  #ifdef BOARD_TTGO_TDISPLAY
    String msg = "TTGO-HiHiHi from node ";
  #else
    String msg = "M5-HiHiHi from node ";
  #endif
  msg += mesh.getNodeId();
  msg += " myFreeMemory: " + String(ESP.getFreeHeap());
  mesh.sendBroadcast(msg);

  if (calc_delay) {
    SimpleList<uint32_t>::iterator node = nodes.begin();
    while (node != nodes.end()) {
      mesh.startDelayMeas(*node);
      node++;
    }
    calc_delay = false;
  }

  Serial.printf("Sending message: %s\n", msg.c_str());
  
  taskSendMessage.setInterval( random(TASK_SECOND * 1, TASK_SECOND * 5));  // between 1 and 5 seconds
}


void receivedCallback(uint32_t from, String & msg) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
}

void newConnectionCallback(uint32_t nodeId) {
  // Reset blink task
  onFlag = false;
  blinkNoNodes.setIterations((mesh.getNodeList().size() + 1) * 2);
  blinkNoNodes.enableDelayed(BLINK_PERIOD - (mesh.getNodeTime() % (BLINK_PERIOD*1000))/1000);
 
  Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
  Serial.printf("--> startHere: New Connection, %s\n", mesh.subConnectionJson(true).c_str());
}

void changedConnectionCallback() {
  Serial.printf("Changed connections\n");
  // Reset blink task
  onFlag = false;
  blinkNoNodes.setIterations((mesh.getNodeList().size() + 1) * 2);
  blinkNoNodes.enableDelayed(BLINK_PERIOD - (mesh.getNodeTime() % (BLINK_PERIOD*1000))/1000);
 
  nodes = mesh.getNodeList();

  Serial.printf("Num nodes: %d\n", nodes.size());
  Serial.printf("Connection list:");

  SimpleList<uint32_t>::iterator node = nodes.begin();
  while (node != nodes.end()) {
    Serial.printf(" %u", *node);
    node++;
  }
  Serial.println();
  calc_delay = true;
}

void nodeTimeAdjustedCallback(int32_t offset) {
  Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(), offset);
}

void delayReceivedCallback(uint32_t from, int32_t delay) {
  Serial.printf("Delay to node %u is %d us\n", from, delay);
}
