String html_template = R"rawliteral(
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
)rawliteral";
