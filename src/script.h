String html_script = R"rawliteral(
<script>
  var ws;
  function initWebSocket() {
    ws = new WebSocket('ws://' + window.location.hostname + ':' + serverPort + '/ws');
    ws.onmessage = function(event) {
      var states = JSON.parse(event.data);
      for (var gpio in states) {
        var gpioElement = document.getElementById("gpio" + gpio);
        if (gpioElement) {
          if (states[gpio]) {
            gpioElement.className = 'indicator-on';
            gpioElement.innerHTML = "";
          } else {
            gpioElement.className = 'indicator-off';
            gpioElement.innerHTML = "";
          }
        }
      }
    };
  }
  window.addEventListener('load', initWebSocket);
</script>
)rawliteral";
