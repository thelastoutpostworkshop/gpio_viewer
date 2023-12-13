#include <WiFi.h>
#include "src/gpio_viewer.h"
#include "secrets.h"

// GPIO Pins monitored using defaults : 50ms interval and showed on port 8080
GPIOViewer gpio_viewer;

int test_digital_pins[] = {5, 4};
const int testDigitalPinsCount = sizeof(test_digital_pins) / sizeof(test_digital_pins[0]);

const int ledChannel = 0;
const int freq = 5000;
const int resolution = 8;
const int ledPin = 22;

void setup()
{
  Serial.begin(115200);

  gpio_viewer.connectToWifi(ssid, password); // If your code aleady include connection to Wifi, you can comment this line
  // gpio_viewer.setPort(5555);             // You can set the http port
  gpio_viewer.setSamplingInterval(100); // You can set the sampling interval in ms
  gpio_viewer.begin();

  // Your own setup code start here
  for (int i = 0; i < testDigitalPinsCount; i++)
  {
    pinMode(test_digital_pins[i], OUTPUT);
  }

  ledcSetup(ledChannel, freq, resolution);
  ledcAttachPin(ledPin, ledChannel);
}

void loop()
{
  for (int i = 0; i < testDigitalPinsCount; i++)
  {
    digitalWrite(test_digital_pins[i], HIGH);
  }
  ledcWrite(ledChannel,255);
  delay(500);
  for (int i = 0; i < testDigitalPinsCount; i++)
  {
    digitalWrite(test_digital_pins[i], LOW);
  }
  ledcWrite(ledChannel,0);
  delay(500);
}
