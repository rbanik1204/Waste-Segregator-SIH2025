/*
 * ESP8266 JSN-SR04T/HC-SR04 Ultrasonic Sensor - DIAGNOSTIC TEST
 * 
 * This is a simplified diagnostic version to test your ultrasonic sensor
 * and verify the wiring is correct.
 */

// Pin configuration
const int TRIG_PIN = 14; // D5 (GPIO14)
const int ECHO_PIN = 12; // D6 (GPIO12)

void setup() {
  Serial.begin(9600);
  delay(500);
  
  Serial.println("\n\n========================================");
  Serial.println("  ULTRASONIC SENSOR DIAGNOSTIC TEST");
  Serial.println("========================================");
  Serial.println();
  Serial.println("Pin Configuration:");
  Serial.print("  TRIG: D5 (GPIO ");
  Serial.print(TRIG_PIN);
  Serial.println(")");
  Serial.print("  ECHO: D6 (GPIO ");
  Serial.print(ECHO_PIN);
  Serial.println(")");
  Serial.println();
  Serial.println("WIRING CHECKLIST:");
  Serial.println("  [1] VCC → 5V (NOT 3.3V)");
  Serial.println("  [2] GND → GND");
  Serial.println("  [3] TRIG → D5");
  Serial.println("  [4] ECHO → D6");
  Serial.println();
  Serial.println("Starting continuous measurements...");
  Serial.println("========================================\n");
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  digitalWrite(TRIG_PIN, LOW);
  
  delay(1000);
}

void loop() {
  // Send trigger pulse
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  
  // Wait for echo
  long duration = pulseIn(ECHO_PIN, HIGH, 30000); // 30ms timeout
  
  Serial.print("Duration: ");
  Serial.print(duration);
  Serial.print(" µs");
  
  if (duration == 0) {
    Serial.println(" → NO ECHO RECEIVED!");
    Serial.println("  Possible causes:");
    Serial.println("  - Sensor not powered (check 5V connection)");
    Serial.println("  - Wrong pin connections");
    Serial.println("  - Faulty sensor");
    Serial.println("  - No object in range (try placing hand 30cm away)");
  } else {
    float distance = duration / 58.0;
    Serial.print(" → Distance: ");
    Serial.print(distance, 1);
    Serial.print(" cm");
    
    if (distance < 2) {
      Serial.println(" (TOO CLOSE - object touching sensor?)");
    } else if (distance < 25) {
      Serial.println(" (Below 25cm minimum)");
    } else if (distance > 450) {
      Serial.println(" (Above 450cm maximum / no object)");
    } else {
      Serial.println(" ✓ VALID READING");
    }
  }
  
  Serial.println();
  delay(1000); // Wait 1 second between readings
}
