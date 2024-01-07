#include <Arduino.h>
#include <gpio_viewer.h>
#include "secrets.h"              // rename the "_secrets.h" file before building

#define DEMO_PIN  18

GPIOViewer gpioViewer;
bool pinState = false;

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);    // send ESP inbuilt log messages to Serial
  
  pinMode(DEMO_PIN, OUTPUT);

  gpioViewer.connectToWifi(WIFI_SSID, WIFI_PASS);
  gpioViewer.setSamplingInterval(125);
  gpioViewer.begin();
}

void loop() {
  pinState = !pinState;
  digitalWrite(DEMO_PIN, pinState);
  log_i("Current pin state: %d", pinState);
  delay(1000);
}