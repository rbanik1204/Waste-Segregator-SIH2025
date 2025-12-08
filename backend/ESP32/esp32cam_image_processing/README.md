# ESP32-CAM: Image Processing Module

## Purpose
This ESP32-CAM module **ONLY** handles image capture and processing. It sends images to the FastAPI backend for YOLOv8n waste classification.

## Hardware
- **Board**: ESP32-CAM (AI-Thinker module recommended)
- **Camera**: OV2640 (built-in)
- **LEDs**: 
  - GPIO 33: Status LED
  - GPIO 4: Hazard indicator LED

## Functionality
1. **Camera Initialization**: Sets up OV2640 camera with optimal settings
2. **Image Capture**: Captures JPEG images every 2 seconds (configurable)
3. **Image Transmission**: Sends images to FastAPI backend `/image` endpoint
4. **Response Processing**: Receives classification results and indicates hazards

## Configuration
Edit these in `esp32cam_image_processing.ino`:
```cpp
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* serverUrl = "http://192.168.1.100:8000/image";
```

## Required Libraries
- `WiFi.h` (built-in)
- `HTTPClient.h` (built-in)
- `esp_camera.h` (built-in)
- `ArduinoJson.h` (optional, for full JSON parsing)

## Installation
1. Install ESP32 board support in Arduino IDE
2. Select board: **AI Thinker ESP32-CAM**
3. Upload code
4. Monitor Serial output (115200 baud)

## Pin Configuration
```
Camera Pins (AI-Thinker ESP32-CAM):
- PWDN: GPIO 32
- RESET: -1 (not used)
- XCLK: GPIO 0
- SIOD: GPIO 26
- SIOC: GPIO 27
- Y9-Y2: GPIO 35, 34, 39, 36, 21, 19, 18, 5
- VSYNC: GPIO 25
- HREF: GPIO 23
- PCLK: GPIO 22
```

## Output
- Serial monitor shows detection results
- Hazard LED blinks when hazardous waste detected
- Status LED indicates WiFi connection

## Notes
- **NO sensors** are connected to this module
- All other sensors are on ESP8266
- Image quality: VGA (640x480) with high JPEG quality
- Capture interval: 2 seconds (adjustable)

