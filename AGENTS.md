# AGENTS: GPIO Viewer

## What this repo is
- Arduino library that serves a web UI (assets hosted at `https://thelastoutpostworkshop.github.io/microcontroller_devkit/gpio_viewer_1_5/`) to monitor GPIO, PWM, ADC, heap, and PSRAM data from ESP32 boards via Async WebServer + Server-Sent Events.
- ESP32 Arduino core **v3+ is required**; older cores intentionally `#error`. Library release string lives in `src/gpio_viewer.h` (`release`) and `library.properties` (`version`).

## Key files and layout
- `src/gpio_viewer.h`: Entire library implementation. Sets up AsyncWebServer + `/events`, traps `ledc*` and `pinMode` calls via macros to detect PWM/pin modes, reads ADC/touch capabilities (unless `NO_PIN_FUNCTIONS` is defined), and builds the minimal HTML shim pointing to hosted assets.
- `gpio_viewer.ino`: Dev/test sketch that exercises GPIO/PWM/analog tasks; uses `secrets.h` for WiFi creds.
- `examples/gpioviewer/gpioviewer.ino`: Minimal usage example expected to ship with the library (keep includes/comments aligned with README).
- `examples/platformio`: PlatformIO example (`platformio.ini`, `src/main.cpp`) showing library usage.
- `sketch.yaml`: Arduino CLI profiles (`next_release`, `user_setup`, `example`) for compile/upload.
- Generated/ephemeral: `build/` outputs (bins, map, compile_commands), `.vscode/` configs; leave untouched unless intentionally regenerating locally.

## Build and upload
- Arduino CLI (preferred for quick compile): from repo root `arduino-cli compile --profile next_release` (uses `sketch.yaml`). Use `--profile example` to validate the shipped example. Upload with `arduino-cli upload --profile <profile>` after confirming the `port` in `sketch.yaml`.
- PlatformIO example: `cd examples/platformio && pio run` or `pio run -t upload` (adjust `default_envs`/board if needed).
- Dependencies are pulled by the toolchains: ESPAsyncWebServer >=3.9.4, AsyncTCP >=3.4.10, SimpleRotary for the dev sketch. Ensure ESP32 core v3.x is installed.

## Coding notes and pitfalls
- Users must include `<gpio_viewer.h>` **first** so the macro wrappers for `pinMode`/`ledcAttach*` hook correctly; changing this breaks pin tracking.
- WiFi mode must be `WIFI_STA`; AP/AP_STA are explicitly unsupported. `connectToWifi` only handles STA.
- SSE endpoints and event names are coupled to the hosted UI: `/events` stream events named `gpio-state`, `free_heap`, and `free_psram`; REST endpoints `/release`, `/sampling`, `/free_psram`, `/espinfo`, `/partition`, `/pinmodes`, and `/pinfunctions` must remain stable unless you also update the frontend.
- `NO_PIN_FUNCTIONS` can be defined before the include to disable ADC/touch detection for boards where function probing causes issues.
- Default HTTP port is `8080`; `setPort` allows override. Sampling interval defaults to `100ms` (`setSamplingInterval` to change).
- If adding features that affect assets, update `baseURL` in `src/gpio_viewer.h` to point at the correct hosted bundle or add bundling logic.
- Keep `release` string and `library.properties` version in sync when cutting releases.

## Validation expectations
- No automated tests. Do a compile check (`arduino-cli compile --profile next_release`) before shipping changes. For library-facing changes, also build the shipped example (`--profile example`) and optionally flash to confirm runtime behaviour.

## Sensitive and local-only data
- `secrets.h` (WiFi credentials) is `.gitignore`d; do not expose or commit its contents. PlatformIO example uses `_secrets.h` as a template.
- Avoid committing `build/` artifacts or `.vscode/` configs; they are local/editor outputs.
