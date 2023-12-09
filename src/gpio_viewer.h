#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <AsyncTCP.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "html.h"
#include "script.h"
#include "boards/esp32_S3_wroom_1.h"
#include "boards/esp32_38pins.h"

// Boards supported
enum BoardType
{
    ESP32_S3_WROOM_1 = 0,
    ESP32_38PINS = 1,
};

// Boards Definitions (must be in the same order as BoardType)
ESPBoard board_models[] = {esp32_S3_wroom_1, esp32_38pins};

class GPIOViewer
{
public:
    GPIOViewer(BoardType boardModel)
    {
        // All pins monitored
        board = &board_models[boardModel];
        numPins = board->getGPIOsCount();
        lastPinStates = new int[numPins];
        gpioPins = board->getGPIOsPins();
    }
    GPIOViewer(const int *pins, int numPins, BoardType boardModel)
        : gpioPins(pins), numPins(numPins)
    {
        board = &board_models[boardModel];
        lastPinStates = new int[numPins];
    }

    ~GPIOViewer()
    {
        ws->closeAll();
        server->end();
        delete[] lastPinStates;
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
        checkWifiStatus();

        server = new AsyncWebServer(port); // Create a new AsyncWebServer instance with the specified port
        ws = new AsyncWebSocket("/ws");

        // Setup WebSocket
        ws->onEvent([this](AsyncWebSocket *server, AsyncWebSocketClient *client,
                           AwsEventType type, void *arg, uint8_t *data, size_t len)
                    { onWebSocketEvent(server, client, type, arg, data, len); });
        server->addHandler(ws);

        // Serve Web Page
        server->on("/", [this](AsyncWebServerRequest *request)
                   { request->send_P(200, "text/html", generateIndexHTML().c_str()); });

        server->begin();

        // Create a task for monitoring GPIOs
        xTaskCreate(&GPIOViewer::monitorTaskStatic, "GPIO Monitor Task", 2048, this, 1, NULL);
    }
    // Create a task for monitoring GPIOs
    static void monitorTaskStatic(void *pvParameter)
    {
        static_cast<GPIOViewer *>(pvParameter)->monitorTask();
    }

private:
    const int *gpioPins;
    int *lastPinStates;
    int numPins;
    uint16_t port = 8080;
    unsigned long samplingInterval = 50;
    AsyncWebServer *server;
    AsyncWebSocket *ws;
    ESPBoard *board;

    void checkWifiStatus(void)
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            Serial.print("GPIO View Web Application URL is: http://");
            Serial.print(WiFi.localIP());
            Serial.print(":");
            Serial.println(port);
            Serial.printf("Board model is: %s\n", board->getBoardModelName().c_str());
        }
        else
        {
            Serial.println("ESP32 is not connected to WiFi.");
        }
    }

    String generateIndexHTML()
    {
        String html = html_template;

        html += "<body>";
        html += "<div class='image-container'>\n";

        html += "<div class='centered-text' id='sampbox'>Sampling Interval is " + String(samplingInterval) + "ms\n" + board->getBoardModelName() + "</div>";
        // Image
        html += "<img src='" + board->getImage() + "' alt='Board Image'>\n";

        for (int i = 0; i < board->getGPIOsCount(); i++)
        {
            int pin = board->getGPIOs()[i].gpio;
            float top = board->getGPIOs()[i].topPosition;
            float left = board->getGPIOs()[i].leftPosition;
            if (pin != -1 && isPinMonitored(pin))
            {
                html += "<div class='indicator-off' style='top:" + String(top) + "%; left: " + String(left) + "%' id='gpio" + String(pin) + "'></div>";
            }
        }

        // Append the port script
        String portScript = "<script>var serverPort = " + String(port) + ";</script>";
        html += portScript;

        // Append the WebSocket script
        html += html_script;

        html += "</div></body></html>";
        return html;
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
            // GPIOs over 32 are read from GPIO_IN1_REG
            return (GPIO.in1.val >> (gpioNum - 32)) & 0x1;
        }
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