#include "src/gpio_viewer.h"
#include <WiFi.h>
#include <SimpleRotary.h>     // Install this library with the Arduino IDE Library Manager
#include "secrets.h"

GPIOViewer gpio_viewer;

#define ROTARY_PIN_A 23
#define ROTARY_PIN_B 22
#define ROTARY_PUSH_BUTTON 22 // Not used
SimpleRotary rotary(ROTARY_PIN_A, ROTARY_PIN_B, ROTARY_PUSH_BUTTON);

int test_digital_pins[] = {33, 25, 26};
const int testDigitalPinsCount = sizeof(test_digital_pins) / sizeof(test_digital_pins[0]);

const int freq = 200;
const int resolution = 16;

struct PWM_PINS
{
  int pin;
  int channel;
  uint16_t level;
};

PWM_PINS test_pwm_pins[] = {{15, 0}, {2, 1}, {0, 2}, {4, 3}};
const int testPWMPinsCount = sizeof(test_pwm_pins) / sizeof(test_pwm_pins[0]);

void setup()
{
  Serial.begin(115200);

  gpio_viewer.connectToWifi(ssid, password); // If your code aleady include connection to Wifi, you can comment this line
  gpio_viewer.setPort(5555);                 // You can set the http port, if not set default port is 8080

  // Example - Your own setup code start here
  test1_setup();

  // Must be at the end of your setup
  // gpio_viewer.setSamplingInterval(100); // You can set the sampling interval in ms
  gpio_viewer.begin();
}

void loop()
{
  test1_loop();
}

void test1_setup()
{
  uint16_t amount = 0;
  for (int i = 0; i < testPWMPinsCount; i++)
  {
    amount += (65535 / testPWMPinsCount);
    ledcSetup(test_pwm_pins[i].channel, freq, resolution);
    ledcAttachPin(test_pwm_pins[i].pin, test_pwm_pins[i].channel);
    test_pwm_pins[i].level = amount;
  }
  for (int i = 0; i < testDigitalPinsCount; i++)
  {
    pinMode(test_digital_pins[i],OUTPUT);
    digitalWrite(test_digital_pins[i],LOW);
  }
  
}
void test1_loop()
{
  for (int i = 0; i < testPWMPinsCount; i++)
  {
    ledcWrite(test_pwm_pins[i].channel, test_pwm_pins[i].level);
    delay(150);
  }
  delay(300);
  for (int i = 0; i < testPWMPinsCount; i++)
  {
    ledcWrite(test_pwm_pins[i].channel, test_pwm_pins[i].level / 2);
    delay(150);
  }
  delay(300);
  readRotaryEncoder();
}

void readRotaryEncoder(void)
{
    byte i;
    i = rotary.rotate();

    if (i == 1)
    {
      Serial.println("Clockwise");
    }

    if (i == 2)
    {
      Serial.println("CounterClockwise");

    }
}
