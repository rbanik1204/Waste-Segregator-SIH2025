/*
 * ESP8266 + LJ12A3-4-Z/BX Inductive Proximity Sensor
 * 
 * Detects metal objects within 4mm range
 * 
 * SENSOR SPECIFICATIONS:
 * ======================
 * LJ12A3-4-Z/BX Inductive Proximity Sensor:
 * - Type: NPN Normally Open (NO)
 * - Operating Voltage: 6-36V DC (typically use 12V or 24V)
 * - Detection Distance: 4mm (detects ferrous/non-ferrous metals)
 * - Output Type: NPN transistor (LOW when metal detected)
 * - Response Frequency: 300Hz
 * - Current Consumption: ≤10mA
 * - Output Current: ≤200mA
 * 
 * WIRING CONFIGURATION:
 * =====================
 * LJ12A3-4-Z/BX has 3 wires:
 *   Brown Wire  → 12V+ (positive supply, from external 12V power supply)
 *   Blue Wire   → GND (common ground with ESP8266)
 *   Black Wire  → Signal Output (via voltage divider to ESP8266)
 * 
 * VOLTAGE DIVIDER (REQUIRED!):
 * ============================
 * Black wire outputs 12V when no metal, 0V when metal detected.
 * ESP8266 GPIO pins are 3.3V max, so we MUST use voltage divider:
 * 
 *   Black (12V) → 10kΩ resistor → D1 (GPIO5) → 4.7kΩ resistor → GND
 * 
 * This creates: 12V × (4.7kΩ / 14.7kΩ) = ~3.8V (safe for ESP8266)
 * 
 * ALTERNATIVE (using relay module or optocoupler):
 * ================================================
 * If you have a relay module or optocoupler, connect Black wire there
 * and use the module's output (3.3V/5V compatible) to ESP8266.
 * 
 * DETECTION LOGIC:
 * ================
 * NPN NO (Normally Open) sensor:
 * - NO METAL: Black wire = 12V (HIGH after voltage divider = ~3.8V)
 * - METAL DETECTED: Black wire = 0V (LOW = GND)
 * 
 * ESP8266 reads:
 * - digitalRead(SENSOR_PIN) == LOW  → Metal detected
 * - digitalRead(SENSOR_PIN) == HIGH → No metal
 */

// ===== Pin Configuration =====
const int SENSOR_PIN = 5;  // D1 (GPIO5) - Connected via voltage divider
const int LED_PIN = 2;     // D4 (GPIO2) - Onboard LED for indication

// ===== Timing Configuration =====
const unsigned long READ_INTERVAL = 100;  // Read every 100ms (fast response)
unsigned long lastReadTime = 0;

// ===== Debounce Configuration =====
const int DEBOUNCE_DELAY = 50;  // 50ms debounce
int lastSensorState = HIGH;
int sensorState = HIGH;
unsigned long lastDebounceTime = 0;

// ===== Detection State =====
bool metalDetected = false;
unsigned long detectionStartTime = 0;

// ===== Setup Function =====
void setup() {
  Serial.begin(9600);
  delay(100);
  
  Serial.println("\n\n========================================");
  Serial.println("  ESP8266 + LJ12A3-4-Z/BX");
  Serial.println("  Inductive Proximity Sensor");
  Serial.println("========================================");
  Serial.println();
  Serial.println("SENSOR TYPE: NPN Normally Open (NO)");
  Serial.println("DETECTION RANGE: 4mm (metal objects)");
  Serial.println();
  
  // Configure pins
  pinMode(SENSOR_PIN, INPUT_PULLUP);  // Enable internal pull-up
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, HIGH);  // LED off initially (active LOW)
  
  // Display wiring info
  Serial.println("WIRING CHECK:");
  Serial.println("  Brown Wire  → 12V+");
  Serial.println("  Blue Wire   → GND (common with ESP8266)");
  Serial.println("  Black Wire  → 10kΩ → D1 → 4.7kΩ → GND");
  Serial.println();
  Serial.println("Pin Configuration:");
  Serial.print("  SENSOR_PIN: D1 (GPIO");
  Serial.print(SENSOR_PIN);
  Serial.println(")");
  Serial.println();
  Serial.println("Starting detection...");
  Serial.println("========================================\n");
  
  delay(1000);
}

// ===== Main Loop =====
void loop() {
  unsigned long currentTime = millis();
  
  // Read sensor with debouncing
  int reading = digitalRead(SENSOR_PIN);
  
  // Check if state changed
  if (reading != lastSensorState) {
    lastDebounceTime = currentTime;
  }
  
  // If reading has been stable for debounce delay
  if ((currentTime - lastDebounceTime) > DEBOUNCE_DELAY) {
    // If state actually changed
    if (reading != sensorState) {
      sensorState = reading;
      
      // NPN NO sensor: LOW = metal detected, HIGH = no metal
      if (sensorState == LOW) {
        // Metal detected
        metalDetected = true;
        detectionStartTime = currentTime;
        digitalWrite(LED_PIN, LOW);  // Turn on LED (active LOW)
        
        Serial.println("╔════════════════════════════════════════╗");
        Serial.println("║       METAL OBJECT DETECTED!           ║");
        Serial.println("╚════════════════════════════════════════╝");
        Serial.print("  Detection Time: ");
        Serial.print(currentTime);
        Serial.println(" ms");
        Serial.println("  Status: METAL PRESENT");
        Serial.println("  Distance: < 4mm");
        Serial.println();
        
      } else {
        // Metal removed
        if (metalDetected) {
          unsigned long detectionDuration = currentTime - detectionStartTime;
          
          Serial.println("----------------------------------------");
          Serial.println("  Metal Object Removed");
          Serial.print("  Detection Duration: ");
          Serial.print(detectionDuration);
          Serial.println(" ms");
          Serial.println("  Status: NO METAL");
          Serial.println("----------------------------------------\n");
        }
        
        metalDetected = false;
        digitalWrite(LED_PIN, HIGH);  // Turn off LED
      }
    }
  }
  
  lastSensorState = reading;
  
  delay(10);  // Small delay for stability
}
