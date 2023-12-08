String html_template = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP32 GPIO State</title>
  <style>
  table {
    width: 100%;
    border-collapse: collapse;
  }
  td {
    border: 1px solid #ddd;
    padding: 8px;
  }
  tr:nth-child(even) {
    background-color: #f2f2f2;
  }
.unmonitored {
  color: gray;
  background-color: #f0f0f0; /* Light gray background */
  text-decoration: line-through; /* Strikethrough effect */
}

</style>
  <script>
    var ws;
    function initWebSocket() {
      ws = new WebSocket('ws://' + window.location.hostname + '/ws');
      ws.onmessage = function(event) {
        var states = JSON.parse(event.data);
        for (var gpio in states) {
          var gpioElement = document.getElementById("gpio" + gpio);
          if (gpioElement) {
            gpioElement.innerHTML = "GPIO " + gpio + ": " + (states[gpio] ? "HIGH" : "LOW");
          }
        }
      };
    }
    window.addEventListener('load', initWebSocket);
  </script>
</head>
<body>
  <h1>ESP32 GPIO Monitor</h1>
)rawliteral";
