#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "html.h"

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

        // Start of the table
        html += "<table>\n";

        // Add GPIO lines dynamically in two columns
        for (int i = 0; i < numPins; i++)
        {
            // Assign the GPIO to a column (odd pins in one column, even in another, for example)
            int column = gpioPins[i] % 2;

            // Start a new row for the first column
            if (column == 0)
            {
                html += "<tr>\n";
            }

            // Add the GPIO pin and state fields
            html += "<td>GPIO " + String(gpioPins[i]) + "</td>";
            html += "<td id='gpio" + String(gpioPins[i]) + "'>Waiting for updates...</td>\n";

            // End the row after the second column
            if (column == 1)
            {
                html += "</tr>\n";
            }
        }

        // Close any open table row and end the table
        html += "</table>\n";

        // Closing tags for HTML
        html += R"rawliteral(
</body>
</html>
)rawliteral";

        return html;
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