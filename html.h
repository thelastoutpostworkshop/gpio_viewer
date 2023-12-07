const char *index_html = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP32 GPIO State</title>
  <script>
    var ws;
    function initWebSocket() {
      ws = new WebSocket('ws://' + window.location.hostname + '/ws');
      ws.onmessage = function(event) {
        var data = JSON.parse(event.data);
        var gpioElement = document.getElementById("gpio" + data.gpio);
        if (gpioElement) {
          gpioElement.innerHTML = "GPIO " + data.gpio + ": " + (data.state ? "HIGH" : "LOW");
        }
      };
    }
    window.addEventListener('load', initWebSocket);
  </script>
</head>
<body>
  <h1>ESP32 GPIO Monitor</h1>
  <p id="gpio5">GPIO 5: Waiting for updates...</p>
  <p id="gpio17">GPIO 17: Waiting for updates...</p>
  <p id="gpio16">GPIO 16: Waiting for updates...</p>
  <!-- Add more <p> tags for additional GPIOs -->
</body>
</html>
)rawliteral";
