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

const String baseURL = "https://thelastoutpostworkshop.github.io/microcontroller_devkit/gpio_viewer/assets/";

#define maxGPIOPins 49

// Global variables to capture PMW pins
const int maxChannels = 64;
int ledcChannelPin[maxChannels][2];
int ledcChannelPinCount = 0;
int ledcChannelResolution[maxChannels][2];
int ledcChannelResolutionCount = 0;

// Macro to trap values pass to ledcAttachPin since there is no ESP32 API
#define ledcAttachPin(pin, channel)                                                                                                                 \
    (ledcChannelPinCount < maxChannels ? ledcChannelPin[ledcChannelPinCount][0] = (pin), ledcChannelPin[ledcChannelPinCount++][1] = (channel) : 0), \
        ledcAttachPin((pin), (channel))

// Macro to trap values pass to ledcSetup since there is no ESP32 API
#define ledcSetup(channel, freq, resolution)                                                                                                                                                  \
    (ledcChannelResolutionCount < maxChannels ? ledcChannelResolution[ledcChannelResolutionCount][0] = (channel), ledcChannelResolution[ledcChannelResolutionCount++][1] = (resolution) : 0), \
        ledcSetup((channel), (freq), (resolution))

enum pinTypes
{
    digitalPin = 0,
    PWMPin = 1
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
        Serial.println("Connecting to WiFi...");
        while (WiFi.status() != WL_CONNECTED)
        {
            delay(500);
            Serial.print(".");
        }
        Serial.println("Connected to WiFi");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
    }

    void begin()
    {
        printPWNTraps();
        checkWifiStatus();

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

        // Serve the main page
        server->on("/", [this](AsyncWebServerRequest *request)
                   { request->send_P(200, "text/html", generateIndexHTML().c_str()); });

        server->begin();

        // Create a task for monitoring GPIOs
        xTaskCreate(&GPIOViewer::monitorTaskStatic, "GPIO Monitor Task", 2048, this, 1, NULL);
    }

    static void monitorTaskStatic(void *pvParameter)
    {
        static_cast<GPIOViewer *>(pvParameter)->monitorTask();
    }

private:
    uint32_t lastPinStates[maxGPIOPins];
    uint16_t port = 8080;
    unsigned long samplingInterval = 50;
    AsyncWebServer *server;
    AsyncEventSource *events;
    size_t freeHeap = 0;
    String freeRAM = formatBytes(ESP.getFreeSketchSpace());

    void checkWifiStatus(void)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            Serial.print("GPIOView Web Application URL is: http://");
            Serial.print(WiFi.localIP());
            Serial.print(":");
            Serial.println(port);
        }
        else
        {
            Serial.println("ESP32 is not connected to WiFi.");
        }
    }

    void printPWNTraps()
    {
        Serial.printf("%d pins are PWM\n", ledcChannelPinCount);
        for (int i = 0; i < ledcChannelPinCount; i++)
        {
            Serial.printf("Pin %d is using channel %d\n", ledcChannelPin[i][0], ledcChannelPin[i][1]);
        }
        Serial.printf("%d channels are used\n", ledcChannelResolutionCount);
        for (int i = 0; i < ledcChannelResolutionCount; i++)
        {
            Serial.printf("Channel %d resolution is %d bits\n", ledcChannelResolution[i][0], ledcChannelResolution[i][1]);
        }
    }

    String generateIndexHTML()
    {
        String html = "<!DOCTYPE HTML><html><head><title>ESP32 GPIO State</title>";

        html += "<base href ='" + baseURL + "'>";
        html += "<link id='defaultStyleSheet' rel='stylesheet' href=''>";
        html += "<link id='boardStyleSheet' rel='stylesheet' href=''>";
        html += "<link rel='icon' href='favicon.ico' type='image/x-icon'>";

        html += "<script src='" + String("script/webSocket.js'></script>");
        html += "<script src='" + String("script/boardSwitcher.js'></script>");
        html += "</head>";

        html += "<body><div class='grid-container'>\n";

        html += "<header class='header'>";
        html += "<div class='centered-text' id='sampbox'>Sampling Interval is " + String(samplingInterval) + " ms</div>";
        html += "</header>";

        // Image
        html += "<div class='image-container'>\n";
        html += "<div id='imageWrapper' class='image-wrapper'>";
        html += "<img id='boardImage' src='' alt='Board Image'>\n";

        html += "<div id='indicators'></div>";

        html += "</div></div></div>";

        // Append the script variables
        String portScript = "<script>var serverPort = " + String(port) + ";</script>";
        html += portScript;

        String eventSource = "<script>var source = new EventSource('http://" + WiFi.localIP().toString() + ":" + String(port) + "/events');</script>";
        html += eventSource;

        html += "<script>var freeSketchSpace = '" + freeRAM + "';</script>";

        html += "</body></html>";
        return html;
    }

    void resetStatePins(void)
    {
        uint32_t originalValue;
        pinTypes pintype;
        Serial.printf("GPIO View Connected, sampling interval is %ums\n", samplingInterval);

        for (int i = 0; i < maxGPIOPins; i++)
        {
            lastPinStates[i] = readGPIO(i, &originalValue, &pintype);
        }
    }

    // Monitor GPIO Values
    void monitorTask()
    {
        uint32_t originalValue;
        pinTypes pintype;
        while (1)
        {
            String jsonMessage = "{";
            bool hasChanges = false;

            for (int i = 0; i < maxGPIOPins; i++)
            {
                int currentState = readGPIO(i, &originalValue, &pintype);
                // Serial.printf("pin=%d, state=%d, original=%u\n",i,currentState,originalValue);

                if (originalValue != lastPinStates[i])
                {
                    if (hasChanges)
                    {
                        jsonMessage += ", ";
                    }
                    jsonMessage += "\"" + String(i) + "\": {\"s\": " + currentState + ", \"v\": " + originalValue + ", \"t\": " + pintype + "}";
                    lastPinStates[i] = currentState;
                    hasChanges = true;
                }
            }

            jsonMessage += "}";

            if (hasChanges)
            {
                sendGPIOStates(jsonMessage);
            }

            size_t heap = esp_get_free_heap_size();
            if (heap != freeHeap)
            {
                freeHeap = heap;
                events->send(formatBytes(freeHeap).c_str(), "free_heap", millis());
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
    int getChannelResolution(int channel)
    {
        for (int i = 0; i < ledcChannelResolutionCount; i++)
        {
            if (ledcChannelResolution[i][0] == channel)
            {
                return ledcChannelResolution[i][1];
            }
        }
        return -1; // Pin not found, return -1 to indicate no channel is associated
    }

    int readGPIO(int gpioNum, uint32_t *originalValue, pinTypes *pintype)
    {
        int channel = getLedcChannelForPin(gpioNum);
        int value;
        if (channel != -1)
        {
            // This is a PWM Pin
            value = mapLedcReadTo8Bit(channel, originalValue);
            *pintype = PWMPin;
            return value;
        }
        // This is a digital pin
        *pintype = digitalPin;
        if (gpioNum < 32)
        {
            // GPIOs 0-31 are read from GPIO_IN_REG
            value = (GPIO.in >> gpioNum) & 0x1;
        }
        else
        {
            // GPIOs over 32 are read from GPIO_IN1_REG
            value = (GPIO.in1.val >> (gpioNum - 32)) & 0x1;
        }
        *originalValue = value;
        if (value == 1)
        {
            return 256;
        }
        return 0;
    }

    int mapLedcReadTo8Bit(int channel, uint32_t *originalValue)
    {
        uint32_t maxDutyCycle = (1 << getChannelResolution(channel)) - 1;
        *originalValue = ledcRead(channel);
        return map(*originalValue, 0, maxDutyCycle, 0, 255);
    }

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