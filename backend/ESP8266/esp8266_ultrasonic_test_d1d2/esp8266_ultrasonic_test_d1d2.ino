/*
 * ESP8266 JSN-SR04T/HC-SR04 - ALTERNATIVE PIN TEST
 * 
 * Uses D1/D2 instead of D5/D6 to test if pin selection is the issue
 */

// Alternative pin configuration
const int TRIG_PIN = 5;  // D1 (GPIO5)
const int ECHO_PIN = 4;  // D2 (GPIO4)

void setup() {
  Serial.begin(9600);
  delay(500);
  
  Serial.println("\n\n========================================");
  Serial.println("  ULTRASONIC TEST - ALTERNATIVE PINS");
  Serial.println("========================================");
  Serial.println();
  Serial.println("Pin Configuration:");
  Serial.print("  TRIG: D1 (GPIO ");
  Serial.print(TRIG_PIN);
  Serial.println(")");
  Serial.print("  ECHO: D2 (GPIO ");
  Serial.print(ECHO_PIN);
  Serial.println(")");
  Serial.println();
  Serial.println("NEW WIRING:");
  Serial.println("  VCC  → 5V/VIN");
  Serial.println("  GND  → GND");
  Serial.println("  TRIG → D1");
  Serial.println("  ECHO → D2");
  Serial.println();
  Serial.println("Starting test...");
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
  long duration = pulseIn(ECHO_PIN, HIGH, 30000);
  
  Serial.print("Duration: ");
  Serial.print(duration);
  Serial.print(" µs");
  
  if (duration == 0) {
    Serial.println(" → NO ECHO");
  } else {
    float distance = duration / 58.0;
    Serial.print(" → ");
    Serial.print(distance, 1);
    Serial.println(" cm ✓");
  }
  
  delay(500);
}
