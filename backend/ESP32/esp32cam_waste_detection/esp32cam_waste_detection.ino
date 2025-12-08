/*
 * ESP32-CAM Waste Detection System
 * 
 * HARDWARE:
 * - ESP32-CAM (AI-Thinker module)
 * - Camera module (OV2640)
 * 
 * FUNCTIONALITY:
 * - Captures images from camera
 * - Sends images to FastAPI backend for YOLOv8n classification
 * - Receives classification results (waste type, hazard status)
 * - LED indicator for hazardous waste detection
 * 
 * NOTE: This ESP32-CAM ONLY handles image processing.
 * All other sensors (GPS, compass, gas sensors, TDS, etc.) are on ESP8266.
 * 
 * CONFIGURATION:
 * - Update WiFi credentials (ssid, password)
 * - Update server URL to your FastAPI backend
 * - Adjust camera pins if using different ESP32-CAM module
 */

#include "WiFi.h"
#include "HTTPClient.h"
#include "esp_camera.h"
#include "ArduinoJson.h"

// === WiFi Configuration ===
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";

// === Backend Server Configuration ===
const char* serverUrl = "http://192.168.1.100:8000/image"; // FastAPI backend URL
const char* detectedUrl = "http://192.168.1.100:8000/detected"; // Poll for latest detection

// === Hardware Configuration ===
const int HAZARD_LED_PIN = 4; // GPIO 4 (built-in LED on some ESP32-CAM boards)
const int CAPTURE_INTERVAL = 2000; // Capture image every 2 seconds

// === Camera Pin Configuration (AI-Thinker ESP32-CAM) ===
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM     0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM       5

#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// === Global Variables ===
unsigned long lastCapture = 0;
unsigned long lastPoll = 0;
const unsigned long POLL_INTERVAL = 1000; // Poll detection status every 1 second

// === Camera Initialization ===
void startCamera() {
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;
  
  // Optimize for waste detection (good quality, reasonable size)
  if(psramFound()){
    config.frame_size = FRAMESIZE_VGA; // 640x480
    config.jpeg_quality = 10; // High quality (lower number = better quality)
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA; // 800x600
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }

  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    delay(1000);
    ESP.restart();
  }
  
  Serial.println("Camera initialized successfully");
}

// === Send Image to Backend ===
String sendImageToBackend(uint8_t *imageBuf, size_t imageLen) {
  HTTPClient http;
  String response = "";
  
  http.begin(serverUrl);
  http.addHeader("Content-Type", "image/jpeg");
  
  int httpResponseCode = http.POST(imageBuf, imageLen);
  
  if (httpResponseCode > 0) {
    response = http.getString();
    Serial.printf("Image sent successfully. Response code: %d\n", httpResponseCode);
    Serial.println("Response: " + response);
  } else {
    Serial.printf("Error sending image. Code: %d\n", httpResponseCode);
  }
  
  http.end();
  return response;
}

// === Parse Classification Response ===
void parseClassificationResponse(String response) {
  // Simple JSON parsing (for ArduinoJson, include library)
  // For now, use string matching
  if (response.indexOf("\"hazard\": 1") >= 0 || response.indexOf("\"hazard\":1") >= 0) {
    Serial.println("⚠️ HAZARDOUS WASTE DETECTED!");
    digitalWrite(HAZARD_LED_PIN, HIGH);
  } else {
    digitalWrite(HAZARD_LED_PIN, LOW);
  }
  
  // Extract waste type
  if (response.indexOf("\"waste_category\"") >= 0) {
    int startIdx = response.indexOf("\"waste_category\":\"") + 18;
    int endIdx = response.indexOf("\"", startIdx);
    if (startIdx > 17 && endIdx > startIdx) {
      String wasteType = response.substring(startIdx, endIdx);
      Serial.println("Waste Type: " + wasteType);
    }
  }
  
  // Extract waste state (dry/wet)
  if (response.indexOf("\"waste_state\"") >= 0) {
    if (response.indexOf("\"dry\"") >= 0) {
      Serial.println("Classification: DRY waste");
    } else if (response.indexOf("\"wet\"") >= 0) {
      Serial.println("Classification: WET waste");
    }
  }
}

// === Poll Detection Status (Optional) ===
void pollDetectionStatus() {
  HTTPClient http;
  http.begin(detectedUrl);
  
  int httpResponseCode = http.GET();
  
  if (httpResponseCode == 200) {
    String response = http.getString();
    Serial.println("Latest Detection: " + response);
  }
  
  http.end();
}

// === Setup ===
void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("\n=== ESP32-CAM Waste Detection System ===");
  Serial.println("Image Processing Only - All sensors on ESP8266");
  
  // Initialize LED
  pinMode(HAZARD_LED_PIN, OUTPUT);
  digitalWrite(HAZARD_LED_PIN, LOW);
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  int tries = 0;
  while (WiFi.status() != WL_CONNECTED && tries < 60) {
    delay(500);
    Serial.print(".");
    tries++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    Serial.print("Server URL: ");
    Serial.println(serverUrl);
  } else {
    Serial.println("\nWiFi connection failed!");
    ESP.restart();
  }
  
  // Initialize camera
  startCamera();
  
  Serial.println("System ready. Starting image capture...");
}

// === Main Loop ===
void loop() {
  // Check WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected. Reconnecting...");
    WiFi.begin(ssid, password);
    delay(5000);
    return;
  }
  
  // Capture and send image at intervals
  if (millis() - lastCapture >= CAPTURE_INTERVAL) {
    Serial.println("\n--- Capturing image ---");
    
    // Capture frame
    camera_fb_t *fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed");
      delay(1000);
      return;
    }
    
    Serial.printf("Image captured: %d bytes\n", fb->len);
    
    // Send to backend
    String response = sendImageToBackend(fb->buf, fb->len);
    
    // Parse response
    if (response.length() > 0) {
      parseClassificationResponse(response);
    }
    
    // Return frame buffer
    esp_camera_fb_return(fb);
    
    lastCapture = millis();
  }
  
  // Poll detection status (optional, for debugging)
  if (millis() - lastPoll >= POLL_INTERVAL) {
    // pollDetectionStatus(); // Uncomment if needed
    lastPoll = millis();
  }
  
  delay(100);
}

