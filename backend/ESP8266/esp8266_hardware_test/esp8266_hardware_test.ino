/*
 * ULTRA-SIMPLE JSN-SR04T Hardware Test
 * Tests if sensor is responding at all
 */

const int TRIG = 14; // D5
const int ECHO = 12; // D6

void setup() {
  Serial.begin(9600);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  digitalWrite(TRIG, LOW);
  delay(1000);
  
  Serial.println("\n=== HARDWARE TEST ===");
  Serial.println("Testing JSN-SR04T connection...\n");
}

void loop() {
  // Trigger
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  
  // Read echo
  long duration = pulseIn(ECHO, HIGH, 30000);
  
  Serial.print("Echo duration: ");
  Serial.print(duration);
  Serial.print(" µs");
  
  if (duration == 0) {
    Serial.println(" ❌ NO RESPONSE - Check wiring!");
    Serial.println("  → Is VCC connected to 5V?");
    Serial.println("  → Is TRIG on D5, ECHO on D6?");
    Serial.println("  → Is GND connected?");
  } else {
    float cm = duration / 58.0;
    Serial.print(" ✓ WORKING - Distance: ");
    Serial.print(cm, 1);
    Serial.println(" cm");
  }
  
  Serial.println();
  delay(1000);
}
