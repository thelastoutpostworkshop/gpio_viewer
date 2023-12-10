#ifndef boards_definitions
#include "boards_def.h"
#endif

const String image3 = "https://thelastoutpostworkshop.github.io/microcontroller_devkit/gpio_viewer/devboards_images/ESP32_D1_R32.png";

//GPIO List and position on the image
BoardGPIO gpios3[] = {
    {0, 45.7, 5},
    {15, 56, 14.5},
    {33, 59.4, 14.5},
    {32, 63, 14.5},
    {2, 76.7, 5},
    {4, 80.2, 5},
    {35, 83.6, 5},
    {34, 87, 5},
    {36, 90.4, 5},
    {39, 94, 5},


    {22, 32, 97},
    {21, 35.4, 97},
    {18, 45.7, 97},
    {19, 49.2, 97},
    {23, 52.5, 97},
    {5, 56, 97},
    {13, 59.5, 97},
    {12, 63, 97},
    {14, 69.8, 97},
    {27, 73.3, 97},
    {16, 76.7, 97},
    {17, 80.2, 97},
    {25, 83.7, 97},
    {26, 87.2, 97},
    {1, 90.5, 97},
    {3, 93.7, 97},

};
const int numGpios3 = sizeof(gpios3) / sizeof(gpios3[0]);

ESPBoard board_esp32_d1_r32(gpios3, numGpios3, image3,"ESP32-D1-R32",1);
