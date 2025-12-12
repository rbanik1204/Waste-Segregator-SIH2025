/*
 * ESP32-CAM Basic Camera Test
 * 
 * Tests ESP32-CAM module with basic camera initialization and capture
 * Compatible with FTDI programmer (USB-to-Serial adapter)
 * 
 * HARDWARE:
 * =========
 * ESP32-CAM AI-Thinker Module
 * - Camera: OV2640 (2MP)
 * - Flash LED on GPIO4
 * - Status LED on GPIO33
 * 
 * FTDI PROGRAMMER CONNECTION:
 * ===========================
 * FTDI → ESP32-CAM
 * GND  → GND
 * 5V   → 5V (use 5V, NOT 3.3V - camera needs power)
 * TX   → U0R (RX pin)
 * RX   → U0T (TX pin)
 * 
 * UPLOAD MODE (Programming):
 * ==========================
 * Connect GPIO0 to GND (enter flash mode)
 * Press RESET button
 * Upload sketch
 * Disconnect GPIO0 from GND
 * Press RESET button (run mode)
 * 
 * RUN MODE (Normal Operation):
 * ============================
 * GPIO0 should be floating (NOT connected to GND)
 * Press RESET to restart
 */

#include "esp_camera.h"

// ===== Camera Pin Configuration (AI-Thinker ESP32-CAM) =====
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27

#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// ===== LED Pins =====
#define FLASH_LED_PIN      4  // Flash LED (bright)
#define STATUS_LED_PIN    33  // Status LED (red)

// ===== Global Variables =====
bool cameraInitialized = false;

void setup() {
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  delay(1000);
  
  Serial.println("\n\n========================================");
  Serial.println("   ESP32-CAM Basic Camera Test");
  Serial.println("========================================");
  Serial.println();
  
  // Configure LED pins
  pinMode(FLASH_LED_PIN, OUTPUT);
  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(FLASH_LED_PIN, LOW);   // Flash off
  digitalWrite(STATUS_LED_PIN, LOW);  // Status LED off
  
  Serial.println("[INFO] ESP32-CAM Module Starting...");
  Serial.print("[INFO] Chip Model: ");
  Serial.println(ESP.getChipModel());
  Serial.print("[INFO] Chip Revision: ");
  Serial.println(ESP.getChipRevision());
  Serial.print("[INFO] CPU Frequency: ");
  Serial.print(ESP.getCpuFreqMHz());
  Serial.println(" MHz");
  Serial.print("[INFO] Free Heap: ");
  Serial.print(ESP.getFreeHeap() / 1024);
  Serial.println(" KB");
  Serial.println();
  
  // Blink status LED to show startup
  Serial.println("[TEST] Status LED Test...");
  for (int i = 0; i < 3; i++) {
    digitalWrite(STATUS_LED_PIN, HIGH);
    delay(200);
    digitalWrite(STATUS_LED_PIN, LOW);
    delay(200);
  }
  
  // Initialize camera
  Serial.println("[INIT] Initializing OV2640 Camera...");
  if (initCamera()) {
    cameraInitialized = true;
    Serial.println("[SUCCESS] Camera initialized successfully!");
    digitalWrite(STATUS_LED_PIN, HIGH);  // Keep LED on when ready
  } else {
    Serial.println("[FAILED] Camera initialization failed!");
    Serial.println("[ERROR] Check camera module connection!");
    blinkError();
  }
  
  Serial.println("========================================");
  Serial.println();
}

void loop() {
  if (!cameraInitialized) {
    Serial.println("[ERROR] Camera not initialized. Retrying in 5 seconds...");
    delay(5000);
    
    if (initCamera()) {
      cameraInitialized = true;
      Serial.println("[SUCCESS] Camera initialized successfully!");
      digitalWrite(STATUS_LED_PIN, HIGH);
    }
    return;
  }
  
  // Capture and display image info every 5 seconds
  Serial.println("\n[CAPTURE] Taking photo...");
  
  // Optional: Turn on flash LED
  digitalWrite(FLASH_LED_PIN, HIGH);
  delay(100);  // Brief flash
  
  camera_fb_t * fb = esp_camera_fb_get();
  
  // Turn off flash
  digitalWrite(FLASH_LED_PIN, LOW);
  
  if (!fb) {
    Serial.println("[ERROR] Camera capture failed!");
    blinkError();
    return;
  }
  
  // Display capture information
  Serial.println("[SUCCESS] Photo captured!");
  Serial.print("  Image Size: ");
  Serial.print(fb->width);
  Serial.print(" x ");
  Serial.println(fb->height);
  Serial.print("  Image Length: ");
  Serial.print(fb->len);
  Serial.println(" bytes");
  Serial.print("  Format: ");
  switch (fb->format) {
    case PIXFORMAT_JPEG:
      Serial.println("JPEG");
      break;
    case PIXFORMAT_RGB565:
      Serial.println("RGB565");
      break;
    case PIXFORMAT_GRAYSCALE:
      Serial.println("Grayscale");
      break;
    default:
      Serial.println("Unknown");
  }
  Serial.print("  Timestamp: ");
  Serial.print(millis());
  Serial.println(" ms");
  
  // Return the frame buffer back to the driver
  esp_camera_fb_return(fb);
  
  Serial.println("[INFO] Frame buffer returned to driver");
  Serial.println("[INFO] Camera ready for next capture");
  
  // Wait before next capture
  delay(5000);
}

// ===== Initialize Camera =====
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
  config.xclk_freq_hz = 20000000;  // 20MHz clock
  config.pixel_format = PIXFORMAT_JPEG;  // JPEG format for easy transmission
  
  // Image quality settings
  // FRAMESIZE options: QQVGA, QCIF, HQVGA, QVGA, CIF, VGA, SVGA, XGA, SXGA, UXGA
  if (psramFound()) {
    Serial.println("[INFO] PSRAM found - using high quality settings");
    config.frame_size = FRAMESIZE_SVGA;    // 800x600
    config.jpeg_quality = 10;              // 0-63 (lower = better quality)
    config.fb_count = 2;                   // Double buffering
  } else {
    Serial.println("[WARN] PSRAM not found - using lower quality settings");
    config.frame_size = FRAMESIZE_VGA;     // 640x480
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  
  // Initialize camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("[ERROR] Camera init failed with error 0x%x\n", err);
    return false;
  }
  
  // Get camera sensor
  sensor_t * s = esp_camera_sensor_get();
  if (s == NULL) {
    Serial.println("[ERROR] Failed to get camera sensor");
    return false;
  }
  
  // Camera sensor settings (optional tuning)
  s->set_brightness(s, 0);     // -2 to 2
  s->set_contrast(s, 0);       // -2 to 2
  s->set_saturation(s, 0);     // -2 to 2
  s->set_special_effect(s, 0); // 0 = No Effect
  s->set_whitebal(s, 1);       // 0 = disable , 1 = enable
  s->set_awb_gain(s, 1);       // 0 = disable , 1 = enable
  s->set_wb_mode(s, 0);        // 0 to 4 - if awb_gain enabled
  s->set_exposure_ctrl(s, 1);  // 0 = disable , 1 = enable
  s->set_aec2(s, 0);           // 0 = disable , 1 = enable
  s->set_ae_level(s, 0);       // -2 to 2
  s->set_aec_value(s, 300);    // 0 to 1200
  s->set_gain_ctrl(s, 1);      // 0 = disable , 1 = enable
  s->set_agc_gain(s, 0);       // 0 to 30
  s->set_gainceiling(s, (gainceiling_t)0);  // 0 to 6
  s->set_bpc(s, 0);            // 0 = disable , 1 = enable
  s->set_wpc(s, 1);            // 0 = disable , 1 = enable
  s->set_raw_gma(s, 1);        // 0 = disable , 1 = enable
  s->set_lenc(s, 1);           // 0 = disable , 1 = enable
  s->set_hmirror(s, 0);        // 0 = disable , 1 = enable
  s->set_vflip(s, 0);          // 0 = disable , 1 = enable
  s->set_dcw(s, 1);            // 0 = disable , 1 = enable
  s->set_colorbar(s, 0);       // 0 = disable , 1 = enable
  
  Serial.println("[INFO] Camera sensor configured");
  Serial.print("[INFO] Frame Size: ");
  Serial.println(config.frame_size);
  Serial.print("[INFO] JPEG Quality: ");
  Serial.println(config.jpeg_quality);
  Serial.print("[INFO] Frame Buffers: ");
  Serial.println(config.fb_count);
  
  return true;
}

// ===== Blink Error Pattern =====
void blinkError() {
  for (int i = 0; i < 5; i++) {
    digitalWrite(STATUS_LED_PIN, HIGH);
    delay(100);
    digitalWrite(STATUS_LED_PIN, LOW);
    delay(100);
  }
}
