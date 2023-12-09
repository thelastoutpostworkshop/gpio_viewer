String html_template = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <title>ESP32 GPIO State</title>
    <style>
      body,
      html {
        margin: 0;
        padding: 0;
        display: flex;
        justify-content: center;
        align-items: center;
        min-height: 100vh; /* Minimum height to fill the viewport */
        background-color: #f0f0f0; /* For better visibility */
      }

      .image-container {
        position: relative;
        display: inline-block;
        max-width: 100%; /* Ensure container doesn't overflow */
        max-height: 100vh; /* Max height to fit in viewport */
        overflow: hidden; /* Hide any overflow */
      }

      .image-container img {
        max-width: 100%; /* Max width to fit in container */
        max-height: 100vh; /* Max height to fit in viewport */
        height: auto; /* Maintain aspect ratio */
        display: block; /* Remove any default inline spacing */
      }

      .indicator-on {
        position: absolute;
        background-color: red;
        width: 1.2vw;
        height: 1.2vw;
          border-radius: 50%;
        padding: 5px;
        transform: translate(-50%, -50%);
      }
      .indicator-off {
        position: absolute;
          background-color: lightgreen;
        width: 1.2vw;
        height: 1.2vw;
          border-radius: 50%;
          transform: translate(-50%, -50%);
        padding: 5px;
        transform: translate(-50%, -50%);
      }
    </style>
</head>
<body>

</body>
</html>
)rawliteral";
