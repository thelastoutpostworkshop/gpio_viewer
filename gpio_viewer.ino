#include <WiFi.h>
#include "src/gpio_viewer.h"
#include "secrets.h"

GPIOViewer gpio_viewer;

int test_digital_pins[] = {5, 4};
const int testDigitalPinsCount = sizeof(test_digital_pins) / sizeof(test_digital_pins[0]);

const int freq = 5000;
const int resolution = 8;

struct PWM_PINS
{
  int pin;
  int channel;
};

PWM_PINS test_pwm_pins[] = {{15, 0}, {2, 1},{0, 2},{4, 3}};
const int testPWMPinsCount = sizeof(test_pwm_pins) / sizeof(test_pwm_pins[0]);
byte amount = 0;

void setup()
{
  Serial.begin(115200);

  gpio_viewer.connectToWifi(ssid, password); // If your code aleady include connection to Wifi, you can comment this line
  gpio_viewer.setPort(5555);                 // You can set the http port, if not set default port is 8080

  // Your own setup code start here
  for (int i = 0; i < testPWMPinsCount; i++)
  {
    ledcSetup(test_pwm_pins[i].channel, freq, resolution);
    ledcAttachPin(test_pwm_pins[i].pin, test_pwm_pins[i].channel);
  }

  // Must be at the end of your setup
  // gpio_viewer.setSamplingInterval(100); // You can set the sampling interval in ms
  gpio_viewer.begin();
}

void loop()
{
  amount = 0;
  for (int i = 0; i < testPWMPinsCount; i++)
  {
    amount+=(255/testPWMPinsCount);
    ledcWrite(test_pwm_pins[i].channel, amount);
    delay(300);
  }
  delay(1000);
  for (int i = 0; i < testPWMPinsCount; i++)
  {
    amount+=(255/testPWMPinsCount);
    ledcWrite(test_pwm_pins[i].channel, 0);
    delay(300);
  }
  delay(1000);
}
