#include "src/gpio_viewer.h"
#include <WiFi.h>
#include <SimpleRotary.h> // Install this library with the Arduino IDE Library Manager
#include "secrets.h"

GPIOViewer gpio_viewer;

struct PWM_PINS
{
  int pin;
  int channel;
  uint16_t level;
};

#define TEST_ESP32_S3

#ifndef TEST_ESP32_S3
#define ROTARY_PIN_A 23
#define ROTARY_PIN_B 22
#define ROTARY_PUSH_BUTTON 22 // Not used
SimpleRotary rotary(ROTARY_PIN_A, ROTARY_PIN_B, ROTARY_PUSH_BUTTON);

int test_digital_pins[] = {33, 25, 26};
const int testDigitalPinsCount = sizeof(test_digital_pins) / sizeof(test_digital_pins[0]);
int currentLed = 0; // Start with the first LED

const int analogPinsCount = 3;
int test_analog_pins[analogPinsCount] = {32, 19, 18};
byte analogValue = 0;

const int freq = 200;
const int resolution = 16;

PWM_PINS test_pwm_pins[] = {{15, 0}, {2, 1}, {0, 2}, {4, 3}};
const int testPWMPinsCount = sizeof(test_pwm_pins) / sizeof(test_pwm_pins[0]);
#else
// Test ESP32-S3
#define INPUT_PIN 1
#define ROTARY_PIN_A 41
#define ROTARY_PIN_B 42
#define ROTARY_PUSH_BUTTON 42 // Not used
SimpleRotary rotary(ROTARY_PIN_A, ROTARY_PIN_B, ROTARY_PUSH_BUTTON);

int test_digital_pins[] = {15, 7, 6};
const int testDigitalPinsCount = sizeof(test_digital_pins) / sizeof(test_digital_pins[0]);
int currentLed = 0; // Start with the first LED

const int analogPinsCount = 2;
int test_analog_pins[analogPinsCount] = {4, 2};
int analogValue = 0;

const int freq = 1000;
const int resolution = 10;

PWM_PINS test_pwm_pins[] = {{17, 0, 0}, {18, 1, 0}, {8, 2, 0}, {3, 3, 0}};
const int testPWMPinsCount = sizeof(test_pwm_pins) / sizeof(test_pwm_pins[0]);
#endif

void setup()
{
  Serial.begin(115200);

  gpio_viewer.connectToWifi(ssid, password);

  test1_setup();

#ifdef TEST_ESP32_S3
  if (psramFound())
  {
    uint8_t *largeMemoryBlock = (uint8_t *)malloc(4 * 1024 * 1024); // 4MB

    if (largeMemoryBlock == nullptr)
    {
      Serial.println("Memory allocation failed!");
    }
    else
    {
      Serial.println("Memory allocation successful.");
    }
  }
#endif

  gpio_viewer.setSamplingInterval(75);
  gpio_viewer.begin();
}

void loop()
{
  test1_loop();
}

uint32_t getMaxDutyCycle(int resolution)
{
  return (1 << resolution) - 1;
}

void test1_setup()
{
  // pinMode(INPUT_PIN,INPUT_PULLUP);
  uint16_t amount = 0;
  for (int i = 0; i < testPWMPinsCount; i++)
  {
    amount += (getMaxDutyCycle(resolution) / testPWMPinsCount);

#if ESP_ARDUINO_VERSION_MAJOR >= 3
    ledcAttach(test_pwm_pins[i].pin, freq, resolution);
#else
    ledcSetup(test_pwm_pins[i].channel, freq, resolution);
    ledcAttachPin(test_pwm_pins[i].pin, test_pwm_pins[i].channel);
#endif

    test_pwm_pins[i].level = amount;
  }
  for (int i = 0; i < analogPinsCount; i++)
  {
    pinMode(test_analog_pins[i], OUTPUT);
  }
  for (int i = 0; i < testDigitalPinsCount; i++)
  {
    pinMode(test_digital_pins[i], OUTPUT);
    digitalWrite(test_digital_pins[i], LOW);
  }
  xTaskCreate(readRotaryEncoderTask, // Task function
              "ReadRotaryEncoder",   // Name of the task (for debugging)
              2048,                  // Stack size (bytes)
              NULL,                  // Parameter to pass to the function
              1,                     // Task priority
              NULL);
}
void test1_loop()
{
  for (int i = 0; i < analogPinsCount; i++)
  {
    analogValue += (i * 3);
    if (analogValue > getMaxDutyCycle(8))
    {
      analogValue = 0;
    }
    analogWrite(test_analog_pins[i], analogValue++);
  }
  for (int i = 0; i < testPWMPinsCount; i++)
  {
#if ESP_ARDUINO_VERSION_MAJOR == 3
    ledcWrite(test_pwm_pins[i].pin, test_pwm_pins[i].level);
#else
    ledcWrite(test_pwm_pins[i].channel, test_pwm_pins[i].level);
#endif
    delay(150);
    test_pwm_pins[i].level += (getMaxDutyCycle(resolution) / 4);
    if (test_pwm_pins[i].level > getMaxDutyCycle(resolution))
    {
      test_pwm_pins[i].level = 0;
    }
  }
  // delay(300);
  // for (int i = 0; i < testDigitalPinsCount; i++)
  // {
  //   if (digitalRead(test_digital_pins[i]) == LOW)
  //   {

  //     digitalWrite(test_digital_pins[i], HIGH);
  //   }
  //   else
  //   {

  //     digitalWrite(test_digital_pins[i], LOW);
  //   }
  // }
  delay(300);
}

void updateLeds()
{
  for (int i = 0; i < testDigitalPinsCount; i++)
  {
    digitalWrite(test_digital_pins[i], i == currentLed ? HIGH : LOW);
  }
}

void readRotaryEncoderTask(void *pvParameters)
{
  for (;;)
  { // Infinite loop
    readRotaryEncoder();
    vTaskDelay(pdMS_TO_TICKS(10)); // Delay for debouncing, adjust as needed
  }
}

void readRotaryEncoder(void)
{
  byte i;
  i = rotary.rotate();

  if (i == 1)
  {
    currentLed = (currentLed - 1 + testDigitalPinsCount) % testDigitalPinsCount;
    updateLeds();
    Serial.println("CounterClockwise");
  }

  if (i == 2)
  {
    currentLed = (currentLed + 1) % testDigitalPinsCount;
    updateLeds();
    Serial.println("Clockwise");
  }
}
