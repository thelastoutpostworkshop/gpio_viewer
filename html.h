String html_template = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP32 GPIO State</title>
  <style>
    .flex-container {
      display: flex;
      align-items: stretch; /* Aligns children (table and image) vertically */
    }
    .flex-item {
      flex: 1; /* Allows the item to grow to fill available space */
    }
    table {
      width: 100%;
      border-collapse: collapse;
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
    img {
      max-height: 100%; /* Resize image to fit the height of the table */
      width: auto; /* Maintain aspect ratio */
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
  <div class="flex-container">
    <div class="flex-item">
      <!-- Left Column Table (Add your table here) -->
    </div>
    <div class="flex-item">
      <img src="http://192.168.1.90/images/esp32_38pins.png" alt="Descriptive Text">
    </div>
    <div class="flex-item">
      <!-- Right Column Table (Add your table here) -->
    </div>
  </div>
</body>
</html>
)rawliteral";
