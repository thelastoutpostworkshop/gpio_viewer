# GPIOViewer Arduino Library to see live GPIO Pins on ESP32 boards

**Transforms the way you troubleshoot your microcontroller projects**.

### Youtube Tutorial

### Installation

- Downnload the code and install the library in the Arduino IDE : `Sketch > Include Library > Add ZIP Library...`
- Install the [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer) in the same way `Sketch > Include Library > Add ZIP Library...`

### Usage
>ℹ️ You can also use get examples provided with the library in the Arduino IDE through the menu `File > Examples > GPIOViewer`

```c
#include <gpio_viewer.h> // Must me the first include in your project
GPIOViewer gpio_viewer;

void setup()
{
  Serial.begin(115200);

  // Comment the next line, If your code aleady include connection to Wifi
  gpio_viewer.connectToWifi(ssid, password);
  // gpio_viewer.setPort(5555);             // You can set the http port

  // Your own setup code start here

  // Must be at the end of your setup
  // gpio_viewer.setSamplingInterval(100); // You can set the sampling interval in ms
  gpio_viewer.begin();
}
```
>ℹ️ The default HTTP port is **8080** and default sampling interval is **50ms**.

### Request an ESP32 board image addition

You can request an ESP32 board image addition by [creating a new issue](https://github.com/thelastoutpostworkshop/gpio_viewer/issues).

### Library Size

The GPIOViewer Library adds xx KB to your projects

### GPIO Supported

- Digital
- PWM
