/*
 * ESP32-CAM: Image Processing Only
 * 
 * Hardware: ESP32-CAM module
 * Purpose: Capture images and send to backend for YOLOv8n waste classification
 * 
 * This ESP32-CAM ONLY handles:
 * - Camera initialization
 * - Image capture
 * - Sending images to FastAPI backend (/image endpoint)
 * - Receiving classification results
 * 
 * All other sensors are on ESP8266
 */

#include "WiFi.h"
#include "HTTPClient.h"
#include "esp_camera.h"
#include "ArduinoJson.h"

// === CONFIGURATION - EDIT THESE ===
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* serverUrl = "http://192.168.1.100:8000/image"; // FastAPI backend URL

// LED pin for visual feedback (GPIO 33 for AI-Thinker ESP32-CAM)
const int STATUS_LED_PIN = 33;
const int HAZARD_LED_PIN = 4; // GPIO 4 for hazard indicator

// Camera configuration for AI-Thinker ESP32-CAM module
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

// Capture interval (milliseconds)
const unsigned long CAPTURE_INTERVAL = 2000; // 2 seconds
unsigned long lastCapture = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("\n=== ESP32-CAM: Image Processing Module ===");
  
  // Initialize LEDs
  pinMode(STATUS_LED_PIN, OUTPUT);
  pinMode(HAZARD_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);
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
    digitalWrite(STATUS_LED_PIN, HIGH);
  } else {
    Serial.println("\nWiFi connection failed!");
    Serial.println("Please check SSID and password");
    // Blink LED to indicate error
    for (int i = 0; i < 5; i++) {
      digitalWrite(STATUS_LED_PIN, HIGH);
      delay(200);
      digitalWrite(STATUS_LED_PIN, LOW);
      delay(200);
    }
  }
  
  // Initialize camera
  if (!initCamera()) {
    Serial.println("Camera initialization failed!");
    ESP.restart();
  }
  
  Serial.println("Camera initialized successfully");
  Serial.println("Ready to capture and process images");
  Serial.println("==========================================\n");
}

bool initCamera() {
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
  
  // High quality for better detection
  if (psramFound()) {
    config.frame_size = FRAMESIZE_VGA; // 640x480
    config.jpeg_quality = 10; // Lower = better quality (10-63)
    config.fb_count = 2;
  } else {
    config.frame_size = FRAMESIZE_SVGA; // 800x600
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x\n", err);
    return false;
  }
  
  return true;
}

String sendImageToServer(uint8_t* imageData, size_t imageLen) {
  HTTPClient http;
  http.begin(serverUrl);
  http.addHeader("Content-Type", "image/jpeg");
  
  Serial.print("Sending image to server... ");
  int httpCode = http.POST(imageData, imageLen);
  String response = "";
  
  if (httpCode > 0) {
    response = http.getString();
    Serial.printf("Success! HTTP Code: %d\n", httpCode);
  } else {
    Serial.printf("Failed! HTTP Code: %d, Error: %s\n", httpCode, http.errorToString(httpCode).c_str());
  }
  
  http.end();
  return response;
}

void parseClassificationResponse(String response) {
  // Parse JSON response from FastAPI
  // Expected format: {"status":"ok","prediction":"plastic:0.89","waste_category":"dry",...}
  
  if (response.length() == 0) {
    return;
  }
  
  // Simple JSON parsing (for basic fields)
  // For full parsing, use ArduinoJson library
  
  // Check for hazardous waste
  if (response.indexOf("\"hazard\":1") >= 0 || response.indexOf("\"hazard\": 1") >= 0) {
    Serial.println("⚠️  HAZARDOUS WASTE DETECTED!");
    digitalWrite(HAZARD_LED_PIN, HIGH);
    delay(1000);
    digitalWrite(HAZARD_LED_PIN, LOW);
  } else {
    digitalWrite(HAZARD_LED_PIN, LOW);
  }
  
  // Extract prediction if available
  int predStart = response.indexOf("\"prediction\":\"");
  if (predStart >= 0) {
    predStart += 14; // Length of "\"prediction\":\""
    int predEnd = response.indexOf("\"", predStart);
    if (predEnd > predStart) {
      String prediction = response.substring(predStart, predEnd);
      Serial.print("Detection: ");
      Serial.println(prediction);
    }
  }
  
  // Extract waste category
  int catStart = response.indexOf("\"waste_category\":\"");
  if (catStart >= 0) {
    catStart += 18; // Length of "\"waste_category\":\""
    int catEnd = response.indexOf("\"", catStart);
    if (catEnd > catStart) {
      String category = response.substring(catStart, catEnd);
      Serial.print("Category: ");
      Serial.println(category);
    }
  }
}

void loop() {
  // Check WiFi connection
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected! Reconnecting...");
    WiFi.begin(ssid, password);
    delay(5000);
    return;
  }
  
  // Capture image at intervals
  if (millis() - lastCapture >= CAPTURE_INTERVAL) {
    lastCapture = millis();
    
    // Capture frame
    camera_fb_t* fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Camera capture failed!");
      return;
    }
    
    Serial.printf("Captured image: %d bytes\n", fb->len);
    
    // Send to server
    String response = sendImageToServer(fb->buf, fb->len);
    
    // Parse and handle response
    parseClassificationResponse(response);
    
    // Return frame buffer
    esp_camera_fb_return(fb);
    
    // Blink status LED to indicate capture
    digitalWrite(STATUS_LED_PIN, LOW);
    delay(50);
    digitalWrite(STATUS_LED_PIN, HIGH);
  }
  
  delay(100); // Small delay to prevent watchdog issues
}

