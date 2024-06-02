#ifndef _GPIOVIEWER_
#define _GPIOVIEWER_
#ifndef WEBSERVER_H
// Compatiblity with the Wifi Manager if your using it in your project
#include <WebServer.h>
#endif
#ifndef WiFi_h
#include <WiFi.h>
#endif
#ifndef _ESPAsyncWebServer_H_
#include <ESPAsyncWebServer.h>
#endif
#ifndef ASYNCTCP_H_
#include <AsyncTCP.h>
#endif
#ifndef INC_FREERTOS_H
#include <freertos/FreeRTOS.h>
#endif
#ifndef INC_TASK_H
#include <freertos/task.h>
#endif
#endif
#include <esp_partition.h>

const char *release = "1.5.4";

const String baseURL = "https://thelastoutpostworkshop.github.io/microcontroller_devkit/gpio_viewer_1_5/";

#if ESP_ARDUINO_VERSION_MAJOR == 3
#include "esp32-hal-periman.h"
#else
extern uint8_t channels_resolution[];
#endif

#define maxGPIOPins 49
#define sentIntervalIfNoActivity 1000L // If no activity for this interval, resend to show connection activity

// Global variables to capture PMW pins
const int maxChannels = 64;
int ledcChannelPin[maxChannels][2];
int ledcChannelPinCount = 0;
int ledcChannelResolution[maxChannels][2];
int ledcChannelResolutionCount = 0;

// Global variables to pins set with PinMode
int pinmode[maxGPIOPins][2];
int pinModeCount = 0;

#if ESP_ARDUINO_VERSION_MAJOR == 3
// Macro to trap values pass to ledcAttach functions since there is no ESP32 API
#define ledcAttach(pin, freq, resolution)                                                                                                                            \
    (ledcChannelPinCount < maxChannels ? ledcChannelPin[ledcChannelPinCount][0] = (pin), ledcChannelPin[ledcChannelPinCount++][1] = (resolution) : 0), \
        ledcAttach((pin), (freq), (resolution))
#define ledcAttachChannel(pin, freq, resolution, channel)                                                                                                            \
    (ledcChannelPinCount < maxChannels ? ledcChannelPin[ledcChannelPinCount][0] = (pin), ledcChannelPin[ledcChannelPinCount++][1] = (resolution) : 0), \
        ledcAttachChannel((pin), (freq), (resolution), (channel))
#define IS_VERSION_3_OR_HIGHER true
#else
// Macro to trap values pass to ledcAttachPin since there is no ESP32 API
#define ledcAttachPin(pin, channel)                                                                                                                 \
    (ledcChannelPinCount < maxChannels ? ledcChannelPin[ledcChannelPinCount][0] = (pin), ledcChannelPin[ledcChannelPinCount++][1] = (channel) : 0), \
        ledcAttachPin((pin), (channel))

// Macro to trap values pass to ledcSetup since there is no ESP32 API
#define ledcSetup(channel, freq, resolution)                                                                                                                                                  \
    (ledcChannelResolutionCount < maxChannels ? ledcChannelResolution[ledcChannelResolutionCount][0] = (channel), ledcChannelResolution[ledcChannelResolutionCount++][1] = (resolution) : 0), \
        ledcSetup((channel), (freq), (resolution))
#endif

// Macro to trap values pass to pinMode since there is no ESP32 API
#define pinMode(pin, mode)                                                                                    \
    (pinModeCount < maxGPIOPins ? pinmode[pinModeCount][0] = (pin), pinmode[pinModeCount++][1] = (mode) : 0), \
        pinMode((pin), (mode))

enum pinTypes
{
    digitalPin = 0,
    PWMPin = 1,
    analogPin = 2
};

class GPIOViewer
{
public:
    GPIOViewer()
    {
    }

    ~GPIOViewer()
    {
        delete events;
        server->end();
    }

    void setPort(uint16_t port)
    {
        this->port = port;
    }

    void setSamplingInterval(unsigned long samplingInterval)
    {
        this->samplingInterval = samplingInterval;
    }

    void connectToWifi(const char *ssid, const char *password)
    {
        WiFi.begin(ssid, password);
        Serial.println("GPIOViewer >> Connecting to WiFi...");
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            Serial.print(".");
        }
        Serial.println("GPIOViewer >> Connected to WiFi");
    }

    void begin()
    {
        Serial.setDebugOutput(false);
        Serial.printf("GPIOViewer >> Release %s\n", release);
#if defined(ESP_ARDUINO_VERSION_MAJOR) && defined(ESP_ARDUINO_VERSION_MINOR) && defined(ESP_ARDUINO_VERSION_PATCH)
        Serial.printf("GPIOViewer >> ESP32 Core Version %d.%d.%d\n", ESP_ARDUINO_VERSION_MAJOR, ESP_ARDUINO_VERSION_MINOR, ESP_ARDUINO_VERSION_PATCH);
        if (ESP_ARDUINO_VERSION_MAJOR < 2)
        {
            Serial.printf("GPIOViewer >> Your ESP32 Core Version is not supported, update your ESP32 boards to the latest version\n");
            return;
        }
#endif
        Serial.printf("GPIOViewer >> Chip Model:%s, revision:%d\n", ESP.getChipModel(), ESP.getChipRevision());
        if (psramFound())
        {
            psramSize = ESP.getPsramSize();
            Serial.printf("GPIOViewer >> PSRAM Size %s\n", formatBytes(psramSize).c_str());
        }
        else
        {
            Serial.printf("GPIOViewer >> No PSRAM\n");
        }

        if (checkWifiStatus())
        {
            // printPWNTraps();
            server = new AsyncWebServer(port);

            // Set CORS headers for global responses
            DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");
            DefaultHeaders::Instance().addHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
            DefaultHeaders::Instance().addHeader("Access-Control-Allow-Headers", "Content-Type");

            // Initialize and set up the AsyncEventSource
            events = new AsyncEventSource("/events");
            events->onConnect([this](AsyncEventSourceClient *client)
                              { this->resetStatePins(); });

            server->addHandler(events);

            server->on("/", [this](AsyncWebServerRequest *request)
                       { request->send_P(200, "text/html", generateIndexHTML().c_str()); });

            server->on("/release", HTTP_GET, [this](AsyncWebServerRequest *request)
                       { sendMinReleaseVersion(request); });

            server->on("/free_psram", HTTP_GET, [this](AsyncWebServerRequest *request)
                       { sendFreePSRAM(request); });

            server->on("/sampling", HTTP_GET, [this](AsyncWebServerRequest *request)
                       { sendSamplingInterval(request); });
            server->on("/espinfo", HTTP_GET, [this](AsyncWebServerRequest *request)
                       { sendESPInfo(request); });
            server->on("/partition", HTTP_GET, [this](AsyncWebServerRequest *request)
                       { sendESPPartition(request); });
            server->on("/pinmodes", HTTP_GET, [this](AsyncWebServerRequest *request)
                       { sendPinModes(request); });

            server->begin();

            Serial.print("GPIOViewer >> Web Application URL is: http://");
            Serial.print(WiFi.localIP());
            Serial.print(":");
            Serial.println(port);

            // Create a task for monitoring GPIOs
            xTaskCreate(&GPIOViewer::monitorTaskStatic, "GPIO Monitor Task", 2048, this, 1, NULL);
        }
    }

    static void monitorTaskStatic(void *pvParameter)
    {
        static_cast<GPIOViewer *>(pvParameter)->monitorTask();
    }

private:
    uint32_t lastPinStates[maxGPIOPins];
    uint16_t port = 8080;
    unsigned long samplingInterval = 100;
    unsigned long lastSentWithNoActivity = millis();
    AsyncWebServer *server;
    AsyncEventSource *events;
    u_int32_t freeHeap = 0;
    u_int32_t freePSRAM = 0;
    u_int32_t psramSize = 0;
    String freeRAM = formatBytes(ESP.getFreeSketchSpace());

    void sendESPPartition(AsyncWebServerRequest *request)
    {
        String jsonResponse = "["; // Start of JSON array
        bool firstEntry = true;    // Used to format the JSON array correctly

        esp_partition_iterator_t iter = esp_partition_find(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, NULL);

        // Loop through partitions
        while (iter != NULL)
        {
            const esp_partition_t *partition = esp_partition_get(iter);

            // Add comma before the next entry if it's not the first
            if (!firstEntry)
            {
                jsonResponse += ",";
            }
            firstEntry = false;

            // Append partition information in JSON format
            jsonResponse += "{";
            jsonResponse += "\"label\":\"" + String(partition->label) + "\",";
            jsonResponse += "\"type\":" + String(partition->type) + ",";
            jsonResponse += "\"subtype\":" + String(partition->subtype) + ",";
            jsonResponse += "\"address\":\"0x" + String(partition->address, HEX) + "\",";
            jsonResponse += "\"size\":" + String(partition->size);
            jsonResponse += "}";

            // Move to next partition
            iter = esp_partition_next(iter);
        }

        // Clean up the iterator
        esp_partition_iterator_release(iter);

        jsonResponse += "]"; // End of JSON array

        request->send(200, "application/json", jsonResponse);
    }

    void sendPinModes(AsyncWebServerRequest *request)
    {
        String jsonResponse = "["; // Start of JSON array
        bool firstEntry = true;    // Used to format the JSON array correctly

        for (int i = 0; i < pinModeCount; i++)
        {
            if (!firstEntry)
            {
                jsonResponse += ",";
            }
            firstEntry = false;
            jsonResponse += "{";
            jsonResponse += "\"pin\":\"" + String(pinmode[i][0]) + "\",";
            jsonResponse += "\"mode\":\"" + String(pinmode[i][1]) + "\"";
            jsonResponse += "}";
        }

        jsonResponse += "]"; // End of JSON array

        request->send(200, "application/json", jsonResponse);
    }
    void sendESPInfo(AsyncWebServerRequest *request)
    {

        // const FlashMode_t flashMode = ESP.getFlashChipMode(); // removed, it crashes with ESP32-S3
        const char* flashMode = "\"not available\"";

        String jsonResponse = "{\"chip_model\": \"" + String(ESP.getChipModel()) + "\",";
        jsonResponse += "\"cores_count\": \"" + String(ESP.getChipCores()) + "\",";
        jsonResponse += "\"chip_revision\": \"" + String(ESP.getChipRevision()) + "\",";
        jsonResponse += "\"cpu_frequency\": \"" + String(ESP.getCpuFreqMHz()) + "\",";
        jsonResponse += "\"cycle_count\": " + String(ESP.getCycleCount()) + ",";
        jsonResponse += "\"mac\": \"" + String(ESP.getEfuseMac()) + "\",";
        jsonResponse += "\"flash_mode\": " + String(flashMode) + ","; 
        jsonResponse += "\"flash_chip_size\": " + String(ESP.getFlashChipSize()) + ",";
        jsonResponse += "\"flash_chip_speed\": " + String(ESP.getFlashChipSpeed()) + ",";
        jsonResponse += "\"heap_size\": " + String(ESP.getHeapSize()) + ",";
        jsonResponse += "\"heap_max_alloc\": " + String(ESP.getMaxAllocHeap()) + ",";
        jsonResponse += "\"psram_size\": " + String(ESP.getPsramSize()) + ",";
        jsonResponse += "\"free_psram\": " + String(ESP.getFreePsram()) + ",";
        jsonResponse += "\"psram_max_alloc\": " + String(ESP.getMaxAllocPsram()) + ",";
        jsonResponse += "\"free_heap\": " + String(ESP.getFreeHeap()) + ",";
        jsonResponse += "\"up_time\": \"" + String(millis()) + "\",";
        jsonResponse += "\"sketch_size\": " + String(ESP.getSketchSize()) + ",";
        jsonResponse += "\"free_sketch\": " + String(ESP.getFreeSketchSpace()) + "";

        jsonResponse += '}';
        request->send(200, "application/json", jsonResponse);
    }

    void sendSamplingInterval(AsyncWebServerRequest *request)
    {
        String jsonResponse = "{\"sampling\": \"" + String(samplingInterval) + "\"}";

        request->send(200, "application/json", jsonResponse);
    }
    void sendMinReleaseVersion(AsyncWebServerRequest *request)
    {
        String jsonResponse = "{\"release\": \"" + String(release) + "\"}";

        request->send(200, "application/json", jsonResponse);
    }
    void sendFreePSRAM(AsyncWebServerRequest *request)
    {
        String jsonResponse = "{\"sampling\": \"" + String(samplingInterval) + "\"}";
        if (psramFound())
        {
            jsonResponse += formatBytes(ESP.getFreePsram()) + "\"}";
        }
        else
        {
            jsonResponse += "No PSRAM\"}";
        }

        request->send(200, "application/json", jsonResponse);
    }
    bool checkWifiStatus(void)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            return true;
        }
        else
        {
            wifi_mode_t mode = WiFi.getMode();

            switch (mode)
            {
            case WIFI_OFF:
                Serial.println("GPIOViewer >> WiFi Mode: OFF");
                break;
            case WIFI_STA:
                Serial.println("GPIOViewer >> WiFi Mode: Station (STA)");
                break;
            case WIFI_AP:
                Serial.println("GPIOViewer >> WiFi Mode: Access Point (AP) is not supported");
                break;
            case WIFI_AP_STA:
                Serial.println("GPIOViewer >> WiFi Mode: Access Point and Station (AP_STA) is not supported");
                break;
            default:
                Serial.println("GPIOViewer >> WiFi Mode: Unknown, cannot run the wep application");
            }
            Serial.println("GPIOViewer >> ESP32 is not connected to WiFi.");
        }
        return false;
    }

    void printPWNTraps()
    {
        Serial.printf("GPIOViewer >> %d pins are PWM\n", ledcChannelPinCount);
        for (int i = 0; i < ledcChannelPinCount; i++)
        {
            Serial.printf("GPIOViewer >> Pin %d is using channel %d\n", ledcChannelPin[i][0], ledcChannelPin[i][1]);
        }
        Serial.printf("GPIOViewer >> %d channels are used\n", ledcChannelResolutionCount);
        for (int i = 0; i < ledcChannelResolutionCount; i++)
        {
            Serial.printf("GPIOViewer >> Channel %d resolution is %d bits\n", ledcChannelResolution[i][0], ledcChannelResolution[i][1]);
        }
        Serial.printf("GPIOViewer >> %d pins have PinMode\n", pinModeCount);
        for (int i = 0; i < pinModeCount; i++)
        {
            Serial.printf("GPIOViewer >> Pin %d is using mode %d\n", pinmode[i][0], pinmode[i][1]);
        }
    }

    String generateIndexHTML()
    {
        String html = "<!DOCTYPE html><html lang='en'><head><meta charset='UTF-8'>";
        html += "<base href ='" + baseURL + "'>";
        html += "<link rel='icon' href='favicon.ico'><meta name='viewport' content='width=device-width, initial-scale=1.0'><title>GPIOViewer</title>";
        html += "<script type='module' crossorigin src='GPIOViewerVue.js'></script>";
        html += "<link rel='stylesheet' crossorigin href='assets/main.css'></head><body><div id='app'></div>";

        html += "<script>";
        html += "window.gpio_settings = {";
        html += "ip:'" + WiFi.localIP().toString() + "',";
        html += "port:'" + String(port) + "',";
        html += "freeSketchRam:'" + freeRAM + "'";
        html += "};";
        html += "</script>";

        html += "</body></html>";
        return html;
    }

    void resetStatePins(void)
    {
        uint32_t originalValue;
        pinTypes pintype;
        Serial.printf("GPIOViewer >> Connected, sampling interval is %lums\n", samplingInterval);

        for (int i = 0; i < maxGPIOPins; i++)
        {
            lastPinStates[i] = readGPIO(i, &originalValue, &pintype);
        }
    }

    // Check GPIO values
    bool checkGPIOValues()
    {
        uint32_t originalValue;
        pinTypes pintype;

        String jsonMessage = "{";
        bool hasChanges = false;

        for (int i = 0; i < maxGPIOPins; i++)
        {
            int currentState = readGPIO(i, &originalValue, &pintype);

            if (originalValue != lastPinStates[i])
            {
                if (hasChanges)
                {
                    jsonMessage += ", ";
                }
                jsonMessage += "\"" + String(i) + "\": {\"s\": " + currentState + ", \"v\": " + originalValue + ", \"t\": " + pintype + "}";
                lastPinStates[i] = originalValue;
                hasChanges = true;
            }
        }

        jsonMessage += "}";

        if (hasChanges)
        {
            sendGPIOStates(jsonMessage);
        }
        return hasChanges;
    }

    bool checkFreeHeap()
    {
        uint32_t heap = esp_get_free_heap_size();
        if (heap != freeHeap)
        {
            freeHeap = heap;
            events->send(formatBytes(freeHeap).c_str(), "free_heap", millis());
            return true;
        }
        return false;
    }

    bool checkFreePSRAM()
    {
        if (psramFound())
        {
            uint32_t psram = ESP.getFreePsram();
            if (psram != freePSRAM)
            {
                freePSRAM = psram;
                events->send(formatBytes(freePSRAM).c_str(), "free_psram", millis());
                return true;
            }
        }
        return false;
    }

    // Monitor Task
    void monitorTask()
    {
        while (1)
        {
            bool changes = false;
            changes = checkGPIOValues();
            changes |= checkFreeHeap();
            changes |= checkFreePSRAM();

            if (!changes)
            {
                unsigned long delay = millis() - lastSentWithNoActivity;
                if (delay > sentIntervalIfNoActivity)
                {
                    // No activity, resending for pulse
                    events->send(formatBytes(freeHeap).c_str(), "free_heap", millis());
                    lastSentWithNoActivity = millis();
                }
            }
            else
            {
                lastSentWithNoActivity = millis();
            }
            vTaskDelay(pdMS_TO_TICKS(samplingInterval));
        }
    }

    int getLedcChannelForPin(int pin)
    {
        for (int i = 0; i < ledcChannelPinCount; i++)
        {
            if (ledcChannelPin[i][0] == pin)
            {
                return ledcChannelPin[i][1];
            }
        }
        return -1; // Pin not found, return -1 to indicate no channel is associated
    }

#if ESP_ARDUINO_VERSION_MAJOR >= 3
    int mapLedcReadTo8Bit(int gpioNum, int channel, uint32_t *originalValue)
    {
        ledc_channel_handle_t *bus = (ledc_channel_handle_t *)perimanGetPinBus(gpioNum, ESP32_BUS_TYPE_LEDC);
        if (bus != NULL)
        {
            uint8_t resolution;
            resolution = bus->channel_resolution;
            uint32_t maxDutyCycle = (1 << resolution) - 1;
            *originalValue = ledcRead(gpioNum);
            return map(*originalValue, 0, maxDutyCycle, 0, 255);
        }
        return 0;
        // if (resolution > 0)
        // {
        //     // Serial.printf("channel=%d,maxDutyCycle=%ld, channel resolution=%d\n", channel, maxDutyCycle, channels_resolution[channel]);
        //     *originalValue = ledcRead(channel);
        //     // Serial.printf("originalValue = %ld\n", *originalValue);
        //     return map(*originalValue, 0, maxDutyCycle, 0, 255);
        // }
        // return 0;
    }
    int readGPIO(int gpioNum, uint32_t *originalValue, pinTypes *pintype)
    {
        int value;
        int channel = getLedcChannelForPin(gpioNum);
        if (channel != -1)
        {
            // This is a PWM Pin
            value = mapLedcReadTo8Bit(gpioNum, channel, originalValue);
            *pintype = PWMPin;
            return value;
        }
        uint32_t ledc_value = ledcRead(gpioNum);
        if (ledc_value != 0)
        {
            value = mapLedcReadTo8Bit(gpioNum,0, originalValue);
            *pintype = analogPin;

            return ledc_value;
        }
        else
        {
            // This is a digital pin
            *pintype = digitalPin;
            value = digitalRead(gpioNum);
            *originalValue = value;
            if (value == 1)
            {
                return 256;
            }
            return 0;
        }
    }
#else
    int mapLedcReadTo8Bit(int gpioNum, int channel, uint32_t *originalValue)
    {
        uint8_t resolution;
        resolution = channels_resolution[channel];
        if (resolution > 0)
        {
            uint32_t maxDutyCycle = (1 << channels_resolution[channel]) - 1;
            // Serial.printf("channel=%d,maxDutyCycle=%ld, channel resolution=%d\n", channel, maxDutyCycle, channels_resolution[channel]);
            *originalValue = ledcRead(channel);
            // Serial.printf("originalValue = %ld\n", *originalValue);
            return map(*originalValue, 0, maxDutyCycle, 0, 255);
        }
        return 0;
    }
    int readGPIO(int gpioNum, uint32_t *originalValue, pinTypes *pintype)
    {
        int channel = getLedcChannelForPin(gpioNum);
        int value;
        if (channel != -1)
        {
            // This is a PWM Pin
            value = mapLedcReadTo8Bit(gpioNum, channel, originalValue);
            *pintype = PWMPin;
            return value;
        }
        uint8_t analogChannel = analogGetChannel(gpioNum);
        if (analogChannel != 0 && analogChannel != 255)
        {
            value = mapLedcReadTo8Bit(gpioNum, analogChannel, originalValue);
            *pintype = analogPin;
            return value;
        }
        else
        {
            // This is a digital pin
            *pintype = digitalPin;
            value = digitalRead(gpioNum);
            *originalValue = value;
            if (value == 1)
            {
                return 256;
            }
            return 0;
        }
    }
#endif

    void sendGPIOStates(const String &states)
    {
        events->send(states.c_str(), "gpio-state", millis());
    }

    String formatBytes(size_t bytes)
    {
        if (bytes < 1024)
        {
            return String(bytes) + " B";
        }
        else if (bytes < (1024 * 1024))
        {
            return String(bytes / 1024.0, 2) + " KB";
        }
        else
        {
            return String(bytes / 1024.0 / 1024.0, 2) + " MB";
        }
    }
};