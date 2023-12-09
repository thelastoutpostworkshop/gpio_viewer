#include <WiFi.h>
#include "src/gpio_monitor.h"
#include "secrets.h"

// Define the GPIO pins to monitor
const int gpioPins[] = {34,35,32,33,25,26,27,14,12,13,9,10,11}; // Example GPIO pins
const int numPins = sizeof(gpioPins) / sizeof(gpioPins[0]);

GPIOMonitor gpio_monitor(50,8080);

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

  gpio_monitor.begin();
}

void loop()
{
  digitalWrite(27, HIGH);
  delay(500);
  digitalWrite(27, LOW);
  delay(500);
}