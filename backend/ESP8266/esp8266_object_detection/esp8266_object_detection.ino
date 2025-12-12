/*
 * ESP8266 JSN-SR04T Waterproof Ultrasonic Sensor - Object Detection
 * 
 * Detects objects using JSN-SR04T ultrasonic distance sensor and displays
 * object presence and proximity on Serial Monitor.
 * 
 * SENSOR SPECIFICATIONS:
 * ======================
 * JSN-SR04T Waterproof Ultrasonic Sensor:
 * - Operating Range: 25cm to 450cm (maximum range used for detection)
 * - Operating Voltage: 5V DC
 * - Ultrasonic Frequency: 40kHz
 * - Waterproof: IP67 rated (fully sealed probe)
 * - Measuring Angle: 75 degrees
 * - Trigger Input Signal: 10µS TTL pulse
 * - Echo Output Signal: TTL pulse proportional to distance
 * 
 * APPLICATION: Pure Object Detection
 * ===================================
 * - Detects presence of objects within 25-450cm range
 * - Reports distance and proximity classification
 * - No fill-level calculation (pure detection mode)
 * - Suitable for obstacle detection, presence sensing, proximity monitoring
 * 
 * HARDWARE WIRING:
 * ================
 * JSN-SR04T Ultrasonic Sensor:
 *   - VCC → 5V (requires 5V power supply)
 *   - GND → GND
 *   - TRIG → ESP8266 D5 (GPIO14) - Trigger pin
 *   - ECHO → ESP8266 D6 (GPIO12) - Echo pin
 * 
 * WIRING NOTES:
 * - ESP8266 GPIO pins are 3.3V tolerant, but JSN-SR04T ECHO output is 5V
 * - Use a voltage divider (2kΩ + 1kΩ resistors) on ECHO pin to protect ESP8266
 * - Voltage Divider: ECHO → 2kΩ → D6 → 1kΩ → GND (reduces 5V to ~1.67V safe range)
 * - OR use a logic level shifter module (recommended for reliability)
 * 
 * ERROR CODES:
 * - Distance = -4: Below minimum range (< 25cm, object too close)
 * - Distance = -5: Above maximum range (> 450cm, no object detected or out of range)
 */

// ===== JSN-SR04T Pin Configuration =====
const int TRIG_PIN = 14; // D5 (GPIO14) - Trigger pin
const int ECHO_PIN = 12; // D6 (GPIO12) - Echo pin (use voltage divider!)

// ===== Sensor Configuration =====
const float MIN_DISTANCE = 5.0;    // Minimum valid distance (cm) - lowered for close detection
const float MAX_DISTANCE = 450.0;  // Maximum valid distance (cm)
const long ECHO_TIMEOUT = 30000;   // Echo timeout in microseconds (30ms)
const int NUM_SAMPLES = 10;        // Increased samples for better accuracy
const float STABILITY_THRESHOLD = 2.0; // cm - ignore changes smaller than this

// ===== Timing Configuration =====
const unsigned long READ_INTERVAL = 2000;  // Read every 2 seconds (faster response)
unsigned long lastReadTime = 0;
float lastValidDistance = -1;      // Store last valid reading for comparison

// ===== Distance Measurement Structure =====
struct DistanceMeasurement {
  float distance_cm;
  bool valid;
  String error_msg;
  int error_code;
};

// ===== Setup Function =====
void setup() {
  Serial.begin(9600);
  delay(100);
  Serial.println("\n\n=== ESP8266 JSN-SR04T Object Detection ===");
  Serial.println("Waterproof Ultrasonic Distance Sensor");
  Serial.println("Detection Range: 5cm - 450cm");
  Serial.println("Mode: Pure Object Detection\n");
  
  // Configure pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);
  
  Serial.println("[INFO] System ready. Starting measurements...\n");
  
  // Test pins
  Serial.println("[TEST] Pin Configuration:");
  Serial.print("  TRIG_PIN (D5/GPIO14): ");
  Serial.println(TRIG_PIN);
  Serial.print("  ECHO_PIN (D6/GPIO12): ");
  Serial.println(ECHO_PIN);
  Serial.println("\n[TEST] Ensure JSN-SR04T is:");
  Serial.println("  - Powered with 5V");
  Serial.println("  - TRIG connected to D5");
  Serial.println("  - ECHO connected to D6 (use voltage divider!)");
  Serial.println("  - Object within 5-450cm range");
  Serial.println("  - NO obstructions in front of sensor probe!\n");
  
  delay(1000);
}

// ===== Main Loop =====
void loop() {
  unsigned long currentTime = millis();
  
  // Check if it's time to read sensor
  if (currentTime - lastReadTime >= READ_INTERVAL) {
    lastReadTime = currentTime;
    
    // Measure distance
    DistanceMeasurement measurement = measureDistance();
    
    // Display measurement
    printMeasurement(measurement);
  }
  
  delay(100);
}

// ===== Measure Distance with JSN-SR04T =====
DistanceMeasurement measureDistance() {
  DistanceMeasurement result;
  result.valid = false;
  result.error_code = 0;
  result.error_msg = "";
  
  // Take multiple samples for median filtering (best noise reduction)
  float distances[NUM_SAMPLES];
  int validSamples = 0;
  
  Serial.println("[DEBUG] Starting measurements...");
  
  for (int i = 0; i < NUM_SAMPLES; i++) {
    // Send 10µs trigger pulse
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(5);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    
    // Read echo pulse duration
    long duration = pulseIn(ECHO_PIN, HIGH, ECHO_TIMEOUT);
    
    Serial.print("[DEBUG] Sample ");
    Serial.print(i + 1);
    Serial.print(": duration = ");
    Serial.print(duration);
    Serial.print(" µs");
    
    if (duration == 0) {
      Serial.println(" - TIMEOUT (no echo)");
      continue;
    }
    
    // Calculate distance: distance = duration / 58.0 (cm)
    float distance = duration / 58.0;
    
    Serial.print(", distance = ");
    Serial.print(distance, 2);
    Serial.print(" cm");
    
    // Validate range
    if (distance >= MIN_DISTANCE && distance <= MAX_DISTANCE) {
      distances[validSamples] = distance;
      validSamples++;
      Serial.println(" - VALID");
    } else {
      Serial.print(" - OUT OF RANGE (");
      Serial.print(MIN_DISTANCE);
      Serial.print("-");
      Serial.print(MAX_DISTANCE);
      Serial.println(" cm)");
    }
    
    delay(60); // Wait 60ms between samples (sensor minimum cycle time)
  }
  
  Serial.print("[DEBUG] Valid samples: ");
  Serial.print(validSamples);
  Serial.print(" out of ");
  Serial.println(NUM_SAMPLES);
  
  // Check if we got enough valid samples (need at least 3)
  if (validSamples < 3) {
    result.error_code = -5;
    result.error_msg = "JSN-SR04T_NO_ECHO_OR_OUT_OF_RANGE";
    result.distance_cm = -5;
    return result;
  }
  
  // Use MEDIAN instead of AVERAGE (removes outliers better)
  // Sort the valid distances
  for (int i = 0; i < validSamples - 1; i++) {
    for (int j = i + 1; j < validSamples; j++) {
      if (distances[i] > distances[j]) {
        float temp = distances[i];
        distances[i] = distances[j];
        distances[j] = temp;
      }
    }
  }
  
  // Get median value (middle element)
  if (validSamples % 2 == 0) {
    result.distance_cm = (distances[validSamples/2 - 1] + distances[validSamples/2]) / 2.0;
  } else {
    result.distance_cm = distances[validSamples/2];
  }
  
  Serial.print("[DEBUG] Median distance: ");
  Serial.print(result.distance_cm, 2);
  Serial.print(" cm (range: ");
  Serial.print(distances[0], 1);
  Serial.print("-");
  Serial.print(distances[validSamples-1], 1);
  Serial.println(" cm)");
  
  // Apply stability filter - ignore small changes
  if (lastValidDistance > 0) {
    float change = abs(result.distance_cm - lastValidDistance);
    if (change < STABILITY_THRESHOLD) {
      Serial.print("[DEBUG] Change ");
      Serial.print(change, 2);
      Serial.println(" cm < threshold, using previous value");
      result.distance_cm = lastValidDistance; // Keep previous stable reading
    }
  }
  
  // Final range validation
  if (result.distance_cm < MIN_DISTANCE) {
    result.error_code = -4;
    result.error_msg = "JSN-SR04T_BELOW_MIN_RANGE";
    result.distance_cm = -4;
    result.valid = false;
  } else if (result.distance_cm > MAX_DISTANCE) {
    result.error_code = -5;
    result.error_msg = "JSN-SR04T_ABOVE_MAX_RANGE";
    result.distance_cm = -5;
    result.valid = false;
  } else {
    result.valid = true;
    lastValidDistance = result.distance_cm; // Store for next comparison
  }
  
  return result;
}

// ===== Classify Detection Proximity =====
String classifyProximity(float distance_cm) {
  if (distance_cm < 0) {
    return "error";
  } else if (distance_cm <= 50) {
    return "immediate";
  } else if (distance_cm <= 100) {
    return "near";
  } else if (distance_cm <= 200) {
    return "moderate";
  } else if (distance_cm <= 350) {
    return "distant";
  } else {
    return "very_distant";
  }
}

// ===== Classify Detection Range =====
String classifyRange(float distance_cm) {
  if (distance_cm < 0) {
    return "error";
  } else if (distance_cm <= 50) {
    return "very_close";
  } else if (distance_cm <= 100) {
    return "close";
  } else if (distance_cm <= 200) {
    return "medium";
  } else if (distance_cm <= 350) {
    return "far";
  } else {
    return "very_far";
  }
}

// ===== Print Measurement =====
void printMeasurement(const DistanceMeasurement& measurement) {
  Serial.println("========================================");
  Serial.println("    JSN-SR04T OBJECT DETECTION");
  Serial.println("========================================");
  
  if (!measurement.valid) {
    Serial.print("  [ERROR] ");
    Serial.println(measurement.error_msg);
    Serial.print("  Error Code: ");
    Serial.println(measurement.error_code);
    Serial.println("  Object Detected: NO");
  } else {
    Serial.print("  Distance: ");
    Serial.print(measurement.distance_cm, 2);
    Serial.println(" cm");
    
    Serial.print("  Range: ");
    Serial.println(classifyRange(measurement.distance_cm));
    
    Serial.print("  Proximity: ");
    Serial.println(classifyProximity(measurement.distance_cm));
    
    Serial.println("  Object Detected: YES");
  }
  
  Serial.println("========================================\n");
}
