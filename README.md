# GPIOViewer Arduino Library to see live GPIO Pins on ESP32 boards

**Transforms the way you troubleshoot your microcontroller projects**.<br>
<a href="https://www.buymeacoffee.com/thelastoutpostworkshop" target="_blank">
    <img src="https://www.buymeacoffee.com/assets/img/custom_images/orange_img.png" alt="Buy Me A Coffee">
</a>



## Youtube Tutorial
[<img src="https://github.com/thelastoutpostworkshop/images/blob/main/GPIO%20Viewer.png" width="300">](https://youtu.be/UxkOosaNohU)
[<img src="https://github.com/thelastoutpostworkshop/images/blob/main/GPIOViewer%20Latest%20Features.png" width="300">](https://youtu.be/JJzRXcQrl3I)

## Installation
### Installation Arduino IDE (Version 2)
>ℹ️ Make sure you have the [latest ESP32 boards](https://github.com/espressif/arduino-esp32)
 by Espressif Systems in your Board Manager<br>

- Install the **GPIOViewer Library with the Arduino IDE Library Manager** or Download the [latest stable release](https://github.com/thelastoutpostworkshop/gpio_viewer/releases/latest) and install the library in the Arduino IDE : `Sketch > Include Library > Add ZIP Library...`
- Download [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer) and install the library in the Arduino IDE `Sketch > Include Library > Add ZIP Library...`
- Install the the [AsyncTCP](https://github.com/dvarrel/AsyncTCP) using the Arduino IDE Library Manager.

### Installation VSCode + PlatformIO

>ℹ️ Make sure you have the [latest ESP32 boards](https://github.com/espressif/arduino-esp32)
 by Espressif Systems in your Platforms<br>

- Install the **GPIOViewer Library using PlateformIO Libraries**

Add (or change) the following to your platformio.ini file:

```
platform = espressif32
framework = arduino
lib_deps =
    https://github.com/dvarrel/AsyncTCP.git
    https://github.com/me-no-dev/ESPAsyncWebServer.git
```

## Usage
>ℹ️ You can also get examples provided with the library in the Arduino IDE through the menu `File > Examples > GPIOViewer`<br>
>ℹ️ You only need to include the library, declare the GPIOViewer and call begin() at the end of your setup, and that's it!<br>
>ℹ️ The URL to the web GPIO viewer application is printed on the serial monitor<br>
```c
#include <gpio_viewer.h> // Must me the first include in your project
GPIOViewer gpio_viewer;

void setup()
{
  Serial.begin(115200);

  // Comment the next line, If your code aleady include connection to Wifi in mode WIFI_STA (WIFI_AP and WIFI_AP_STA are not supported)
  gpio_viewer.connectToWifi("Your SSID network", "Your WiFi Password");
  // gpio_viewer.setPort(5555);   // You can set the http port, if not set default port is 8080

  // Your own setup code start here

  // Must be at the end of your setup
  // gpio_viewer.setSamplingInterval(25); // You can set the sampling interval in ms, if not set default is 100ms
  gpio_viewer.begin();
}
```
>ℹ️ The default HTTP port is **8080** and default sampling interval is **100ms**<br>
>ℹ️ Wifi must be in mode WIFI_STA (WIFI_AP and WIFI_AP_STA are not supported) 

## GPIO Supported

- Digital
- Analog
- PWM

## Library Size

- The GPIOViewer Library adds 50 KB to your projects.
- No worries!  All the assets (ex. board images) of the web application are loaded from github pages and don't add to the size of your projects.

## ESP32 Core SDK Compatibility
- The ESP32 Arduino Core that is installed in your system will need to be V2.0.5 or greater, in order for GPIO viewer to compile properly.
- See the official Espressif Systems ESP32 Core documentation located here for more details: https://github.com/espressif/arduino-esp32

## Performance
- Ensure you have a strong Wifi signal with a good transfer rate.  25ms sampling interval works great on Wifi 6 with 125 Mbps.
- If you get "ERROR: Too many messages queued" on the Serial Monitor, this means the data is not read fast enough by the web application.  The data will still be displayed, but with some latency.  Reduce the sampling interval or try to improve your Wifi performance.

## Contributors
Contributors are welcomed!  If you want to submit pull requests, [here is how you can do it](https://docs.github.com/en/get-started/exploring-projects-on-github/contributing-to-a-project).

## Troubleshooting
### Code not compiling
If your code don't compile, **before submitting an issue:**
- Compile with the [latest stable release](https://github.com/thelastoutpostworkshop/gpio_viewer/releases/latest) of the GPIOViewer Library **and** with the [latest ESP32 boards](https://github.com/espressif/arduino-esp32) 
- See also this [solved issue](https://github.com/thelastoutpostworkshop/gpio_viewer/issues/116)
### GPIOViewer running
If GPIOViewer is running and your are experiencing problems in the web application, **before submitting an issue:**
- Make sure you are using the [latest stable release](https://github.com/thelastoutpostworkshop/gpio_viewer/releases/latest) of the GPIOViewer Library
- Clear your browser cache data and refresh the window in your browser

## ESP32 Boards Supported
>ℹ️ You can use the "Generic View" in the GPIO Web Application to see GPIO pin activites live even if your board image is not listed <br>
>ℹ️ You can also request an ESP32 board image addition by [creating a new issue](https://github.com/thelastoutpostworkshop/gpio_viewer/issues).


| Description        | Image                               | Pinout                               |
|--------------------|-------------------------------------|-------------------------------------|
| AZ Delivery NodeMCU ESP32 | ! <img src="https://github.com/thelastoutpostworkshop/microcontroller_devkit/blob/main/gpio_viewer_1_5/devboards_images/AZ-Delivery-NodeMCU-ESP32.png" width="100">             | <img src="https://raw.githubusercontent.com/thelastoutpostworkshop/images/main/AZ-Delivery-NodeMCU-ESP32-p%C3%AEnout.png" width="100">|
| ESP32 VROOM 32D (38 pins)  | ! <img src="https://github.com/thelastoutpostworkshop/microcontroller_devkit/blob/main/gpio_viewer_1_5/devboards_images/ESP32-VROOM-32D.png" width="100">             | <img src="https://docs.espressif.com/projects/esp-idf/en/latest/esp32/_images/esp32-devkitC-v4-pinout.png" width="100">|
| ESP32 VROOM 32D (30 pins)   | ! <img src="https://github.com/thelastoutpostworkshop/microcontroller_devkit/blob/main/gpio_viewer_1_5/devboards_images/ESP32-VROOM-32D-30pins.png" width="100">             |<img src="https://raw.githubusercontent.com/AchimPieters/esp32-homekit-camera/master/Images/ESP32-30PIN-DEVBOARD.png" width="100">|
| ESP32 D1 R32  | ! <img src="https://github.com/thelastoutpostworkshop/microcontroller_devkit/blob/main/gpio_viewer_1_5/devboards_images/ESP32_D1_R32.png" width="100">             |<img src="https://ito-iot.jp/sbc/WeMosD1R32-pinout.png" width="100">  |
| ESP32-CAM  | ! <img src="https://github.com/thelastoutpostworkshop/microcontroller_devkit/blob/main/gpio_viewer_1_5/devboards_images/ESP32-CAM.png" width="100">             |<img src="https://i0.wp.com/randomnerdtutorials.com/wp-content/uploads/2020/03/ESP32-CAM-pinout-new.png?quality=100&strip=all&ssl=1" width="100">|
| ESP32-C3 Super Mini  | ! <img src="https://github.com/thelastoutpostworkshop/microcontroller_devkit/blob/main/gpio_viewer_1_5/devboards_images/ESP32-C3-Super-Mini.png" width="100">             |<img src="https://europe1.discourse-cdn.com/arduino/original/4X/8/0/f/80f07e53b7ddd448a8ac00608dd82acb5611a201.jpeg" width="100">|
| ESP32 C3 Wroom-02  | ! <img src="https://github.com/thelastoutpostworkshop/microcontroller_devkit/blob/main/gpio_viewer_1_5/devboards_images/esp32-C3-Wroom-02.png" width="100">             |<img src="https://m.media-amazon.com/images/S/aplus-media-library-service-media/a80e998c-f2b4-4c4a-9ee7-d97ede61e14c.__CR0,0,970,600_PT0_SX970_V1___.jpg" width="100">|
| ESP32-C6 DevKitM  | ! <img src="https://github.com/thelastoutpostworkshop/microcontroller_devkit/blob/main/gpio_viewer_1_5/devboards_images/esp32-C6-devkitm.png" width="100">             |<img src="https://docs.espressif.com/projects/espressif-esp-dev-kits/en/latest/_images/esp32-c6-devkitm-1-pin-layout.png" width="100">|
| ESP32 Wroom-32UE  | ! <img src="https://github.com/thelastoutpostworkshop/microcontroller_devkit/blob/main/gpio_viewer_1_5/devboards_images/esp32-Wroom-32UE.png" width="100">             |<img src="https://media.springernature.com/lw685/springer-static/image/chp%3A10.1007%2F978-3-031-02447-4_74/MediaObjects/519315_1_En_74_Fig2_HTML.png" width="100">|
| ESP32 EVB  | ! <img src="https://github.com/thelastoutpostworkshop/microcontroller_devkit/blob/main/gpio_viewer_1_5/devboards_images/ESP32-EVB.png" width="100">             |<img src="https://gitlab.com/gschorcht/RIOT.wiki-Images/raw/master/esp32/Olimex_ESP32-EVB_pinout.png" width="100">|
| ESP32 S3 Wroom-1  | ! <img src="https://github.com/thelastoutpostworkshop/microcontroller_devkit/blob/main/gpio_viewer_1_5/devboards_images/esp32-s3-wroom-1.png" width="100">             |<img src="https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/_images/ESP32-S3_DevKitC-1_pinlayout_v1.1.jpg" width="100">|
| Esp32 S2 Mini V1.0.0  | ! <img src="https://github.com/thelastoutpostworkshop/microcontroller_devkit/blob/main/gpio_viewer_1_5/devboards_images/Esp32 S2 Mini V1.0.0.png" width="100">             |<img src="https://www.studiopieters.nl/wp-content/uploads/2022/08/WEMOS-ESP32-S2-MINI.png" width="100">|
| ESP32 POE  | ! <img src="https://github.com/thelastoutpostworkshop/microcontroller_devkit/blob/main/gpio_viewer_1_5/devboards_images/ESP32-POE.png" width="100">             |<img src="https://www.olimex.com/Products/IoT/ESP32/ESP32-POE/resources/ESP32-POE-GPIO.png" width="100">|
| ESP32 C3 Mini  | ! <img src="https://github.com/thelastoutpostworkshop/microcontroller_devkit/blob/main/gpio_viewer_1_5/devboards_images/ESP32-C3-Mini.png" width="100">             |<img src="https://www.wemos.cc/en/latest/_static/boards/c3_mini_v2.1.0_4_16x9.png" width="100">|
| ESP32 C3 Zero  | ! <img src="https://github.com/thelastoutpostworkshop/microcontroller_devkit/blob/main/gpio_viewer_1_5/devboards_images/ESP32-C3-Zero.png" width="100">             |<img src="https://www.waveshare.com/img/devkit/ESP32-C3-Zero/ESP32-C3-Zero-details-inter.jpg" width="100">|
| ESP32 Pico Kit v4.1  | ! <img src="https://github.com/thelastoutpostworkshop/microcontroller_devkit/blob/main/gpio_viewer_1_5/devboards_images/esp32-pico-kit-v4.1.png" width="100">             |<img src="https://docs.espressif.com/projects/esp-idf/en/latest/esp32/_images/esp32-pico-kit-v4-pinout.png" width="100">|
| Leaf S3  | ! <img src="https://github.com/thelastoutpostworkshop/microcontroller_devkit/blob/main/gpio_viewer_1_5/devboards_images/Leaf-S3.png" width="100">             |<img src="https://wiki.banana-pi.org/images/7/7e/Leaf-S3_board.png" width="100">|
| Lilygo T-SIM A7670x | ! <img src="https://github.com/thelastoutpostworkshop/microcontroller_devkit/blob/main/gpio_viewer_1_5/devboards_images/Lilygo-T-SIM-A7670x.png" width="100">             |<img src="https://www.lilygo.cc/cdn/shop/products/LILYGO-T-SIMA7670E_10.jpg?v=1669962736&width=1445" width="100">|
| Lilygo T7 Mini32 v1.5  | ! <img src="https://github.com/thelastoutpostworkshop/microcontroller_devkit/blob/main/gpio_viewer_1_5/devboards_images/Lilygo-T7-Mini32-v1.5.png" width="100">             |<img src="https://templates.blakadder.com/assets/device_images/lilygo_T7_pinout.webp" width="100">|
| Lolin D32 | ! <img src="https://github.com/thelastoutpostworkshop/microcontroller_devkit/blob/main/gpio_viewer_1_5/devboards_images/Lolin-D32.png" width="100">             |<img src="https://mischianti.org/wp-content/uploads/2022/10/ESP32-WeMos-LOLIN-D32-pinout-mischianti-1024x618.jpg.webp" width="100">|
| Freenove ESP32-S3 | ! <img src="https://github.com/thelastoutpostworkshop/microcontroller_devkit/blob/main/gpio_viewer_1_5/devboards_images/Freenove-esp32-s3.png" width="100">             |<img src="https://taunoerik.files.wordpress.com/2023/05/esp32s3_pinout.png?w=1568" width="100">|
| Freenove ESP32-Wroom | ! <img src="https://github.com/thelastoutpostworkshop/microcontroller_devkit/blob/main/gpio_viewer_1_5/devboards_images/Freenove-esp32-wroom.png" width="100">             ||
| Nano ESP32  | ! <img src="https://github.com/thelastoutpostworkshop/microcontroller_devkit/blob/main/gpio_viewer_1_5/devboards_images/Nano-ESP32.png" width="100">             |<img src="https://docs.arduino.cc/static/8307a46e73a4b5b153aa9ec0a4082443/ABX00083-pinout.png" width="100">|
| Sailor Hat ESP32  | ! <img src="https://github.com/thelastoutpostworkshop/microcontroller_devkit/blob/main/gpio_viewer_1_5/devboards_images/Sailor-Hat-ESP32.png" width="100">             |<img src="https://docs.hatlabs.fi/sh-esp32/media/sh-esp32_r2.0.0_top_render.jpg" width="100">|
| SparkleIoT ESP32-C3F  | ! <img src="https://github.com/thelastoutpostworkshop/microcontroller_devkit/blob/main/gpio_viewer_1_5/devboards_images/Sparkleiot-ESP32-C3F.png" width="100">             |<img src="https://raw.githubusercontent.com/thelastoutpostworkshop/images/main/SparkleIoT-ESP32-C3F-pinout.png" width="100">|
| StickLite-V3-ESP32S3  | ! <img src="https://github.com/thelastoutpostworkshop/microcontroller_devkit/blob/main/gpio_viewer_1_5/devboards_images/StickLite-V3-ESP32S3.png" width="100">             |<img src="https://heltec.org/wp-content/uploads/2023/09/HTIT-WSL_V3.png" width="100">|
| T-Display S3 AMOLED | ! <img src="https://github.com/thelastoutpostworkshop/microcontroller_devkit/blob/main/gpio_viewer_1_5/devboards_images/T-Display-S3-AMOLED.png" width="100">             |<img src="https://raw.githubusercontent.com/Xinyuan-LilyGO/T-Display-S3-AMOLED/main/image/T-Display-S3-AMOLED.jpg" width="100">|
| TinyPICO Nano  | ! <img src="https://github.com/thelastoutpostworkshop/microcontroller_devkit/blob/main/gpio_viewer_1_5/devboards_images/TinyPICO-Nano.png" width="100">             |<img src="https://d2j6dbq0eux0bg.cloudfront.net/images/90477757/3788332370.jpg" width="100">|
| TinyPico V3  | ! <img src="https://github.com/thelastoutpostworkshop/microcontroller_devkit/blob/main/gpio_viewer_1_5/devboards_images/TinyPico-V3.png" width="100">             |<img src="https://images.squarespace-cdn.com/content/v1/5c85d89877b903606126e6df/c4a98965-7415-414e-8358-339e3a0ce837/thumb-tinypico-v3_pinout_1200px.jpg?format=2500w" width="100">|
| TTGO Display V1.1  | ! <img src="https://github.com/thelastoutpostworkshop/microcontroller_devkit/blob/main/gpio_viewer_1_5/devboards_images/TTGO-Display-V1.1.png" width="100">             |<img src="https://uelectronics.com/wp-content/uploads/2020/10/Esquematico-TTGO-T-Display-ESP32-scaled.jpg" width="100">|
| Wemos Lolin32 Lite V1  | ! <img src="https://github.com/thelastoutpostworkshop/microcontroller_devkit/blob/main/gpio_viewer_1_5/devboards_images/Wemos-Lolin32-Lite-V1.png" width="100">             |<img src="https://templates.blakadder.com/assets/device_images/wemos_LOLIN32_Lite_v1_pinout.webp" width="100">|
| Wemos Lolin S3 Mini  | ! <img src="https://github.com/thelastoutpostworkshop/microcontroller_devkit/blob/main/gpio_viewer_1_5/devboards_images/Wemos-Lolin-S3-Mini.png" width="100">             |<img src="https://universalsolder.b-cdn.net/wp-content/uploads/2023/05/26837-WEMOS-S3-Mini-ESP32-S3FH4R2-1.jpg" width="100">|
| Wemos D1 Mini ESP32  | ! <img src="https://github.com/thelastoutpostworkshop/microcontroller_devkit/blob/main/gpio_viewer_1_5/devboards_images/Wemos-D1-Mini-ESP32.png" width="100">             |<img src="https://templates.blakadder.com/assets/device_images/wemos_D1_Mini_ESP32_pinout.webp" width="100">|
| Wemos D1 Mini ESP8266  | ! <img src="https://github.com/thelastoutpostworkshop/microcontroller_devkit/blob/main/gpio_viewer_1_5/devboards_images/Wemos-D1-Mini-ESP8266.png" width="100">             |<img src="https://edistechlab.com/wp-content/uploads/2021/04/WeMos-d1-mini-Pin-out.png" width="100">|
| WT32-S1-ETH01  | ! <img src="https://github.com/thelastoutpostworkshop/microcontroller_devkit/blob/main/gpio_viewer_1_5/devboards_images/WT32-S1.png" width="100">             |<img src="https://uelectronics.com/wp-content/uploads/2023/12/AR3833-WT32-ETH01-Placa-desarrollo-pinout.jpg" width="100">|
| NodeMcu ESP8266 | ! <img src="https://github.com/thelastoutpostworkshop/microcontroller_devkit/blob/main/gpio_viewer_1_5/devboards_images/NodeMcu-ESP8266.png" width="100">             |<img src="https://i0.wp.com/randomnerdtutorials.com/wp-content/uploads/2019/05/ESP8266-NodeMCU-kit-12-E-pinout-gpio-pin.png?resize=817%2C542&quality=100&strip=all&ssl=1" width="100">|
| XIAO ESP32 C3  | ! <img src="https://github.com/thelastoutpostworkshop/microcontroller_devkit/blob/main/gpio_viewer_1_5/devboards_images/XIAO-ESP32-C3.png" width="100">             |<img src="https://files.seeedstudio.com/wiki/XIAO_WiFi/pin_map-2.png" width="100">|
| XIAO ESP32 S3  | ! <img src="https://github.com/thelastoutpostworkshop/microcontroller_devkit/blob/main/gpio_viewer_1_5/devboards_images/XIAO-ESP32-S3.png" width="100">             |<img src="https://cdn.cnx-software.com/wp-content/uploads/2023/03/XIAO-ESP32-S3-pinout-diagram.png?lossy=0&strip=none&ssl=1" width="100">|
