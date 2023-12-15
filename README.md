# GPIOViewer Arduino Library to see live GPIO Pins on ESP32 boards
**Transforms the way you troubleshoot your microcontroller projects**. 

### Youtube Tutorial

### Installation
Downnload the code and install the library in the Arduino IDE : Sketch > Include Library > Add ZIP Library... 

### Usage
```c
#include <gpio_viewer.h> // Must me the first include in your project
GPIOViewer gpio_viewer;

void setup()
{
  Serial.begin(115200);

  gpio_viewer.connectToWifi(ssid, password); // If your code aleady include connection to Wifi, you can comment this line
  // gpio_viewer.setPort(5555);             // You can set the http port

  // Your own setup code start here

  // Must be at the end of your setup
  // gpio_viewer.setSamplingInterval(100); // You can set the sampling interval in ms
  gpio_viewer.begin();
}

```

### Library Size
The GPIOViewer Library adds xx KB to your projects

### GPIO Supported
Digital
PWM