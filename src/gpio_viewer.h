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
const String defaultCSS = "css/default.css";

#define maxPins 49

const int maxChannels = 64;
int ledcChannelPin[maxChannels][2];        // Array to store channel and pin pairs
int ledcChannelPinCount = 0;               // Counter to keep track of the number of pairs stored
int ledcChannelResolution[maxChannels][2]; // Array to store channel and resolution
int ledcChannelResolutionCount = 0;        // Counter to keep track of the number of pairs stored

// Macro to trap values pass to ledcAttachPin since there is no ESP32 API
#define ledcAttachPin(pin, channel)                                                                                                                 \
    (ledcChannelPinCount < maxChannels ? ledcChannelPin[ledcChannelPinCount][0] = (pin), ledcChannelPin[ledcChannelPinCount++][1] = (channel) : 0), \
        ledcAttachPin((pin), (channel))

// Macro to trap values pass to ledcSetup since there is no ESP32 API
#define ledcSetup(channel, freq, resolution)                                                                                                                                                  \
    (ledcChannelResolutionCount < maxChannels ? ledcChannelResolution[ledcChannelResolutionCount][0] = (channel), ledcChannelResolution[ledcChannelResolutionCount++][1] = (resolution) : 0), \
        ledcSetup((channel), (freq), (resolution))

class GPIOViewer
{
public:
    GPIOViewer()
    {
    }

    ~GPIOViewer()
    {
        ws->closeAll();
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
        ws = new AsyncWebSocket("/ws");

        ws->onEvent([this](AsyncWebSocket *server, AsyncWebSocketClient *client,
                           AwsEventType type, void *arg, uint8_t *data, size_t len)
                    { onWebSocketEvent(server, client, type, arg, data, len); });
        server->addHandler(ws);

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
    int lastPinStates[maxPins];
    uint16_t port = 8080;
    unsigned long samplingInterval = 50;
    AsyncWebServer *server;
    AsyncWebSocket *ws;

    void checkWifiStatus(void)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            Serial.print("GPIO View Web Application URL is: http://");
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
            Serial.printf("Channel %d resolutions is %d bits\n", ledcChannelResolution[i][0], ledcChannelResolution[i][1]);
        }
    }

    String generateIndexHTML()
    {
        String html = "<!DOCTYPE HTML><html><head><title>ESP32 GPIO State</title>";

        html += "<base href ='" + baseURL + "'>";
        html += "<link rel='stylesheet' href='" + defaultCSS + "'>";
        html += "<link id='boardStyleSheet' rel='stylesheet' href='css/esp32_default.css'>";

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

        html += "</body></html>";
        return html;
    }

    void resetStatePins(void)
    {
        for (int i = 0; i < maxPins; i++)
        {
            lastPinStates[i] = -1; // Initialize with an invalid state
        }
    }

    void monitorTask()
    {
        while (1)
        {
            String jsonMessage = "{";
            bool hasChanges = false;

            for (int i = 0; i < maxPins; i++)
            {
                int currentState = readGPIO(i);
                if (currentState != lastPinStates[i])
                {
                    if (hasChanges)
                    {
                        jsonMessage += ", ";
                    }
                    jsonMessage += "\"" + String(i) + "\": " + currentState;
                    lastPinStates[i] = currentState;
                    hasChanges = true;
                }
            }

            jsonMessage += "}";

            if (hasChanges)
            {
                sendGPIOStates(jsonMessage);
            }

            vTaskDelay(pdMS_TO_TICKS(samplingInterval));
        }
    }

    int getLedcChannelForPin(int pin)
    {
        for (int i = 0; i < ledcChannelPinCount; i++)
        {
            if (ledcChannelPin[i][0] == pin)
            {                                // Check if the pin matches
                return ledcChannelPin[i][1]; // Return the corresponding channel
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
    int readGPIO(int gpioNum)
    {
        int channel = getLedcChannelForPin(gpioNum);
        int value;
        if (channel != -1)
        {
            // This is a PWM Pin
            // uint32_t value = ledcRead(channel);
            value = mapLedcReadTo8Bit(channel);
            // Serial.printf("channel %d mapValue=%u\n", channel, mapValue);

            return value;
        }
        // This is a digital pin
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
        if (value == 1)
        {
            return 256;
        }
        return 0;
    }

    int mapLedcReadTo8Bit(int channel)
    {
        uint32_t maxDutyCycle = (1 << getChannelResolution(channel)) - 1;
        // Serial.printf("channel %d maxDutyCycle=%u\n",channel,maxDutyCycle);
        uint32_t dutyCycle = ledcRead(channel);
        // Serial.printf("channel %d dutyCycle=%u\n",channel,dutyCycle);
        return map(dutyCycle, 0, maxDutyCycle, 0, 255);
    }

    void sendGPIOStates(const String &states)
    {
        ws->textAll(states);
    }

    void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
                          AwsEventType type, void *arg, uint8_t *data, size_t len)
    {
        if (type == WS_EVT_CONNECT)
        {
            Serial.printf("GPIO View Activated, sampling interval is %ums\n", samplingInterval);
            resetStatePins();
        }
        else if (type == WS_EVT_DISCONNECT)
        {
            Serial.println("GPIO View Stopped");
        }
    }
};