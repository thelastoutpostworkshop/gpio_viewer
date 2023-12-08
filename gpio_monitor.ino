#include <WiFi.h>
#include "gpio_monitor.h"
#include "secrets.h"

// Define the GPIO pins to monitor
const int gpioPins[] = {1, 5, 8, 17, 16, 9, 10, 11}; // Example GPIO pins
const int numPins = sizeof(gpioPins) / sizeof(gpioPins[0]);

GPIOMonitor gpio_monitor(gpioPins, numPins);

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

  pinMode(5, OUTPUT);

  gpio_monitor.begin();
}

void loop()
{
  digitalWrite(5, HIGH);
  delay(500);
  digitalWrite(5, LOW);
  delay(500);
}
