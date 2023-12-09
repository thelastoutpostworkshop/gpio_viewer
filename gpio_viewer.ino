#include <WiFi.h>
#include "src/gpio_viewer.h"
#include "secrets.h"

// You can view GPIO pins of choice using defaults:  50ms interval and showed on port 8080
// const int gpioPins[] = {13,9,10,11}; // Example GPIO pins
// const int numPins = sizeof(gpioPins) / sizeof(gpioPins[0]);
// GPIOViewer gpio_viewer(gpioPins,4);

// All GPIO Pins monitored using defaults : 50ms interval and showed on port 8080
GPIOViewer gpio_viewer;

#define test_pin 48
#define test_pin36 36 // Blinking when 48 is used

void setup()
{
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  pinMode(test_pin, OUTPUT);
  pinMode(test_pin36, OUTPUT);
  digitalWrite(test_pin36, LOW);

  // gpio_viewer.setPort(5555);         // You can set the http port
  gpio_viewer.setSamplingInterval(100); // You can set the sampling interval in ms
  gpio_viewer.begin();
}

void loop()
{
  digitalWrite(test_pin, HIGH);
  delay(500);
  digitalWrite(test_pin, LOW);
  delay(500);
}
