this is a port of the esp32 idf bluetooth a2dp example so that it can be run in the Arduino IDE

This is the original code:
https://github.com/espressif/esp-idf/tree/master/examples/bluetooth/a2dp_source/main

The major changes are that the bluetooth controller must be initialised differently (see app_main in main.c).

I also changed all the log messages to ESP_LOGE so they would print on the serial console.

Apart from that it's pretty much the same.

Feb 2019
andrew.stuart@supercoders.com.au
