#include <WiFi.h>
#include "src/gpio_viewer.h"
#include "secrets.h"

//All GPIO Pins monitored using 100ms interval and showed on port 5555
//GPIOViewer gpio_viewer(100,5555);

// You can view GPIO pins of choice using defaults:  50ms interval and showed on port 8080
// const int gpioPins[] = {13,9,10,11}; // Example GPIO pins
// const int numPins = sizeof(gpioPins) / sizeof(gpioPins[0]);
// GPIOViewer gpio_viewer(gpioPins,4);

//All GPIO Pins monitored using defaults : 50ms interval and showed on port 8080
GPIOViewer gpio_viewer;

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

  pinMode(27, OUTPUT);

  gpio_viewer.begin();
}

void loop()
{
  digitalWrite(27, HIGH);
  delay(500);
  digitalWrite(27, LOW);
  delay(500);
}
