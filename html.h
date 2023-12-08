String html_template = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP32 GPIO State</title>
  <style>
    .grid-container {
      display: grid;
      grid-template-columns: 1fr 1fr 1fr; /* Three equal-width columns */
      gap: 10px; /* Space between columns */
    }
    .grid-item {
      display: flex;
      flex-direction: column; /* Stack child elements vertically */
      align-items: center; /* Center items horizontally */
    }
    .grid-item img {
      max-width: 100%; /* Limit width to parent's width */
      height: auto; /* Maintain aspect ratio */
      object-fit: contain; /* Scale the image to fit */
    }
    table {
      width: 100%;
      border-collapse: collapse;
      height: 100%;
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
    .high {
    background-color: red;
  }

  .low {
    background-color: transparent;
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
          if (states[gpio]) {
            gpioElement.className = 'high';
          } else {
            gpioElement.className = 'low';
          }
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
