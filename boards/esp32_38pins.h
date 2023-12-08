#include "borards_def.h"

const String image = "http://192.168.1.90/images/esp32_38pins.png";

BoardGPIO gpios[] = {
    {34, 27, 6.2},
    {35, 31.5, 6.2},
    {32, 36.5, 6.2},
    {33, 41, 6.2},
    {25, 45.5, 6.2},
    {26, 50.3, 6.2},
    {27, 55, 6.2},
    {14, 59.5, 6.2},
    {12, 64, 6.2},
    {13, 73.2, 6.2},
    {9, 78, 6.2},
    {10, 82.5, 6.2},
    {11, 87, 6.2},
};
const int numGpios = sizeof(gpios) / sizeof(gpios[0]);

ESPBoard esp32(gpios, numGpios, image);
