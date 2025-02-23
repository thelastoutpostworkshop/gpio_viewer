/***
This example is intended to demonstrate the use of the GPIO Viewer Library.

Tutorial : https://youtu.be/UxkOosaNohU
Latest Features : https://youtu.be/JJzRXcQrl3I
Documentation : https://github.com/thelastoutpostworkshop/gpio_viewer

// Last tested on:
// Espressif Arduino Core v3.1.3
// ESP Async WebServer 3.7.1
// AsyncTCP            3.3.5
***/

// Since version 1.5.6, the library detects pin functions like ADC and Touch, this has been causing problems on some boards, like the XiaoESP32-S3-Sense. You can disable pin detection by uncommenting the following line:
// #define NO_PIN_FUNCTIONS

#include <gpio_viewer.h> // Must me the first include in your project
GPIOViewer gpio_viewer;

void setup()
{
  Serial.begin(115200);

  // Comment the next line, If your code aleady include connection to Wifi in mode WIFI_STA (WIFI_AP and WIFI_AP_STA are not supported)
  gpio_viewer.connectToWifi("Your SSID network", "Your WiFi Password");
  // gpio_viewer.setPort(5555);                 // You can set the http port, if not set default port is 8080

  // Your own setup code start here

  // Must be at the end of your setup
  // gpio_viewer.setSamplingInterval(25); // You can set the sampling interval in ms, if not set default is 100ms
  gpio_viewer.begin();
}

// You don't need to change your loop function
void loop() {

}
// The rest of your code here