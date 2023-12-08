String html_template = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP32 GPIO State</title>
  <style>
    .flex-container {
      display: flex;
      align-items: stretch; /* Stretch items to fill the container height */
    }
    .flex-item {
      flex: 1; /* Flex items share the container space */
      display: flex;
      flex-direction: column; /* Stack child elements vertically */
    }
    .flex-item img {
      width: auto; /* Maintain aspect ratio */
      height: 100%; /* Stretch to fill the parent height */
      object-fit: scale-down; /* Scale down if needed to fit in the container */
    }
    table {
      width: 100%;
      border-collapse: collapse;
      margin-bottom: auto; /* Push the table to the top */
    }
    td {
      border: 1px solid #ddd;
      padding: 8px;
    }
    .unmonitored {
      color: gray;
      background-color: #f0f0f0;
      text-decoration: line-through;
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
            gpioElement.innerHTML =(states[gpio] ? "HIGH" : "LOW");
          }
        }
      };
    }
    window.addEventListener('load', initWebSocket);
  </script>
</head>
<body>
  <h1>ESP32 GPIO Monitor</h1>
</body>
</html>
)rawliteral";
