#include "main.c"
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif


void setup() {
  // put your setup code here, to run once:
  app_main();

}

void loop() {
  // put your main code here, to run repeatedly:

}
