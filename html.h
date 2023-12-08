String html_template = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP32 GPIO State</title>
  <style>
  .flex-container {
    display: flex;
    align-items: stretch; /* Align items vertically */
  }
  .flex-item {
    flex: 1; /* Flex items share the container space */
    display: flex;
    flex-direction: column; /* Stack child elements vertically */
  }
  .flex-item img {
    max-width: 100%; /* Limit width to parent's width */
    height: auto; /* Maintain aspect ratio */
    max-height: 100%; /* Limit height to parent's height */
    object-fit: contain; /* Do not stretch the image */
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
