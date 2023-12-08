#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "html.h"

const int leftColumnPins[] = {36, 39, 34, 35, 32, 33, 25, 26, 27, 14, 12, 13, 9, 10, 11};
const int rightColumnPins[] = {23, 22, 1, 3, 21, 18, 18, 5, 17, 16, 4, 0, 2, 15};
const int numLeftColumnPins = sizeof(leftColumnPins) / sizeof(leftColumnPins[0]);
const int numRightColumnPins = sizeof(rightColumnPins) / sizeof(rightColumnPins[0]);

class GPIOMonitor
{
public:
    GPIOMonitor(const int *pins, int numPins, unsigned long samplingInterval = 50)
        : gpioPins(pins), numPins(numPins), samplingInterval(samplingInterval), server(80), ws("/ws")
    {
        lastPinStates = new int[numPins];
    }

    ~GPIOMonitor()
    {
        delete[] lastPinStates;
    }

    void begin()
    {

        // Setup WebSocket
        ws.onEvent([this](AsyncWebSocket *server, AsyncWebSocketClient *client,
                          AwsEventType type, void *arg, uint8_t *data, size_t len)
                   { onWebSocketEvent(server, client, type, arg, data, len); });
        server.addHandler(&ws);

        // Serve Web Page
        server.on("/", [this](AsyncWebServerRequest *request)
                  { request->send_P(200, "text/html", generateIndexHTML().c_str()); });

        server.begin();
        // Create a task for monitoring GPIOs
        xTaskCreate(&GPIOMonitor::monitorTaskStatic, "GPIO Monitor Task", 2048, this, 1, NULL);
    }
    // Create a task for monitoring GPIOs
    static void monitorTaskStatic(void *pvParameter)
    {
        static_cast<GPIOMonitor *>(pvParameter)->monitorTask();
    }

private:
    const int *gpioPins;
    int *lastPinStates;
    int numPins;
    unsigned long samplingInterval;
    AsyncWebServer server;
    AsyncWebSocket ws;

    String generateIndexHTML()
    {
        String html = html_template;

        // Start of the flex container
        html += "<div class='flex-container'>\n";

        // Left column table
        html += "<div class='flex-item'>\n<table>\n";
        for (int i = 0; i < numLeftColumnPins; i++)
        {
            html += "<tr>\n";
            addPinToHTML(html, leftColumnPins[i]);
            html += "</tr>\n";
        }
        html += "</table>\n</div>\n";

        // Image
        html += "<div class='flex-item'>\n";
        html += "<img src='http://192.168.1.90/images/esp32_38pins.png' alt='ESP32 Image'>\n";
        html += "</div>\n";

        // Right column table
        html += "<div class='flex-item'>\n<table>\n";
        for (int i = 0; i < numRightColumnPins; i++)
        {
            html += "<tr>\n";
            addPinToHTML(html, rightColumnPins[i]);
            html += "</tr>\n";
        }
        html += "</table>\n</div>\n";

        // End of the flex container
        html += "</div>\n";

        html += "</body></html>";
        return html;
    }

    void addPinToHTML(String &html, int pin)
    {
        if (pin != -1 && isPinMonitored(pin))
        {
            // html += "<td>GPIO " + String(pin) + "</td>";
            html += "<td id='gpio" + String(pin) + "'>Waiting for updates...</td>";
        }
        else
        {
            html += "<td class='unmonitored'>-</td>";
            // html += "<td class='unmonitored'>Unmonitored</td>";
        }
    }

    bool isPinMonitored(int pin)
    {
        for (int i = 0; i < numPins; i++)
        {
            if (gpioPins[i] == pin)
            {
                return true;
            }
        }
        return false;
    }

    void resetStatePins(void)
    {
        for (int i = 0; i < numPins; i++)
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

            for (int i = 0; i < numPins; i++)
            {
                int currentState = readGPIORegister(gpioPins[i]);
                if (currentState != lastPinStates[i])
                {
                    if (hasChanges)
                    {
                        jsonMessage += ", ";
                    }
                    jsonMessage += "\"" + String(gpioPins[i]) + "\": " + (currentState ? "1" : "0");
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

    int readGPIORegister(int gpioNum)
    {
        if (gpioNum < 32)
        {
            // GPIOs 0-31 are read from GPIO_IN_REG
            return (GPIO.in >> gpioNum) & 0x1;
        }
        else
        {
            // GPIOs 32-39 are read from GPIO_IN1_REG
            return (GPIO.in1.val >> (gpioNum - 32)) & 0x1;
        }
    }

    void sendGPIOStates(const String &states)
    {
        ws.textAll(states);
    }

    void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
                          AwsEventType type, void *arg, uint8_t *data, size_t len)
    {
        if (type == WS_EVT_CONNECT)
        {
            Serial.println("WebSocket client connected");
            resetStatePins();
        }
        else if (type == WS_EVT_DISCONNECT)
        {
            Serial.println("WebSocket client disconnected");
        }
    }
};