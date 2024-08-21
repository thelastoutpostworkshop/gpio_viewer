// Test Code for the GPIOViewer development
// Last tested on:
// Espressif Arduino Core v3.0.4
// ESP Async WebServer 3.0.6
// AsyncTCP            1.1.4
//
#include "src/gpio_viewer.h"
#include <WiFi.h>
#include <SimpleRotary.h>
#include "secrets.h"

GPIOViewer gpio_viewer;

struct PWM_PINS
{
  int pin;
  int channel;
  uint16_t level;
};

// #define TEST_ESP32_S3

#ifndef TEST_ESP32_S3
#define ROTARY_PIN_A 23
#define ROTARY_PIN_B 22
#define ROTARY_PUSH_BUTTON 22 // Not used
#define SLOW_PWM_PIN 27
#define SLOW_PMW_CHANNEL 5

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

#define SLOW_PWM_PIN 20

SimpleRotary rotary(ROTARY_PIN_A, ROTARY_PIN_B, ROTARY_PUSH_BUTTON);

int test_digital_pins[] = {15, 4, 2};
const int testDigitalPinsCount = sizeof(test_digital_pins) / sizeof(test_digital_pins[0]);
int currentLed = 0; // Start with the first LED

const int analogPinsCount = 1;
int test_analog_pins[analogPinsCount] = {7};
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

void slowPWMPin(void *pvParameters)
{
  // Setup
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcAttach(SLOW_PWM_PIN, 5000, 8);
  uint8_t slow_level = 0;
#else
  ledcSetup(SLOW_PMW_CHANNEL, 5000, 8);
  ledcAttachPin(SLOW_PWM_PIN, SLOW_PMW_CHANNEL);
  uint8_t slow_level = 0;
  // Serial.printf("SLOW_PWM_PIN=%d\n", SLOW_PWM_PIN);
#endif

  // Loop
  for (;;)
  { // Infinite loop
    // Serial.printf("ledcWrite=%d\n", slow_level);
    ledcWrite(SLOW_PMW_CHANNEL, slow_level += 20);
    delay(2000);
  }
}

void TestPWMPin(void *pvParameters)
{
  // Setup
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

  // Loop
  for (;;)
  {
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
  }
}

void TestDigitalPin(void *pvParameters)
{
  // Setup
  for (int i = 0; i < testDigitalPinsCount; i++)
  {
    pinMode(test_digital_pins[i], OUTPUT);
    digitalWrite(test_digital_pins[i], LOW);
  }

  // Loop
  while (true)
  {
    for (int i = 0; i < testDigitalPinsCount; i++)
    {
      if (digitalRead(test_digital_pins[i]) == LOW)
      {

        digitalWrite(test_digital_pins[i], HIGH);
      }
      else
      {
        digitalWrite(test_digital_pins[i], LOW);
      }
    }
    delay(300);
  }
}
void TestAnalogPin(void *pvParameters)
{
  // Setup

  // Loop
  while (true)
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

    delay(300);
  }
}

void test1_setup()
{
  // pinMode(INPUT_PIN,INPUT_PULLUP);

  // xTaskCreate(readRotaryEncoderTask, "ReadRotaryEncoder", 2048, NULL, 1, NULL);
  xTaskCreate(slowPWMPin, "slowPWMPin", 2048, NULL, 1, NULL);
  xTaskCreate(TestPWMPin, "TestPWMPin", 2048, NULL, 1, NULL);
  xTaskCreate(TestDigitalPin, "TestDigitalPin", 2048, NULL, 1, NULL);
  xTaskCreate(TestAnalogPin, "TestAnalogPin", 2048, NULL, 1, NULL);
}
void test1_loop()
{
  delay(1);
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
