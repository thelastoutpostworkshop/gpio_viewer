String css_template = R"rawliteral(
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
        width: var(--indicator-size);
        height: var(--indicator-size);
          border-radius: 50%;
        padding: 5px;
        transform: translate(-50%, -50%);
      }
      .indicator-off {
        position: absolute;
          background-color: lightgreen;
        width: var(--indicator-size);
        height: var(--indicator-size);
          border-radius: 50%;
          transform: translate(-50%, -50%);
        padding: 5px;
        transform: translate(-50%, -50%);
      }
      .centered-text {
        position: absolute;
        top: 50%;
        left: 50%;
        transform: translate(-50%, -50%);
        color: #f9f5f5; /* Text color */
        font-size: 1vw; /* Responsive font size */
        text-align: center;
        background-color: rgba(8, 145, 213, 0.9); /* Blue background with 90% opacity */
        padding: 10px; /* Padding around the text */
        border-radius: px; /* Rounded corners */
        border: 5px solid darkblue; /* Red border */
        font-weight: bold;
        font-family: Arial, sans-serif; /* Arial font */
      }
)rawliteral";
