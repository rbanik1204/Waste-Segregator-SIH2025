/*
 * Dual Conveyor Belt - WiFi Control
 * Control motors via WiFi app using simple commands
 * 
 * Commands:
 *   1F - Motor 1 Forward
 *   1B - Motor 1 Backward
 *   1S - Motor 1 Stop
 *   2F - Motor 2 Forward
 *   2B - Motor 2 Backward
 *   2S - Motor 2 Stop
 *   AF - All Forward
 *   AS - All Stop
 */

#include <ESP8266WiFi.h>

// ===== WiFi AP Configuration =====
const char* WIFI_SSID = "ConveyorControl";
const char* WIFI_PASSWORD = "12345678";

WiFiServer server(80);

// ===== Motor 1 Pins =====
const int ENA = D5;
const int IN1 = D1;
const int IN2 = D2;

// ===== Motor 2 Pins =====
const int ENB = D6;
const int IN3 = D3;
const int IN4 = D4;

// Motor control functions
void motor1Forward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  Serial.println("Motor 1: FORWARD");
}

void motor1Backward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  Serial.println("Motor 1: BACKWARD");
}

void motor1Stop() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  Serial.println("Motor 1: STOP");
}

void motor2Forward() {
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  Serial.println("Motor 2: FORWARD");
}

void motor2Backward() {
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  Serial.println("Motor 2: BACKWARD");
}

void motor2Stop() {
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  Serial.println("Motor 2: STOP");
}

void allForward() {
  motor1Forward();
  motor2Forward();
  Serial.println("ALL: FORWARD");
}

void allStop() {
  motor1Stop();
  motor2Stop();
  Serial.println("ALL: STOP");
}

void setup() {
  Serial.begin(9600);
  delay(1000);
  
  Serial.println();
  Serial.println("====================================");
  Serial.println("Dual Conveyor Belt - WiFi Control");
  Serial.println("====================================");
  Serial.println();
  
  // Set all pins as OUTPUT
  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  
  // Enable both motors (full speed)
  digitalWrite(ENA, HIGH);
  digitalWrite(ENB, HIGH);
  
  // Stop both motors initially
  allStop();
  
  // Start WiFi Access Point
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(IPAddress(192, 168, 4, 1), IPAddress(192, 168, 4, 1), IPAddress(255, 255, 255, 0));
  WiFi.softAP(WIFI_SSID, WIFI_PASSWORD);
  
  IPAddress ip = WiFi.softAPIP();
  Serial.println("WiFi AP Started");
  Serial.print("SSID: ");
  Serial.println(WIFI_SSID);
  Serial.print("Password: ");
  Serial.println(WIFI_PASSWORD);
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.println();
  
  // Start TCP server
  server.begin();
  Serial.println("Server started on port 80");
  Serial.println("Ready for connections!");
  Serial.println("====================================");
}

void loop() {
  WiFiClient client = server.available();
  
  if (client) {
    Serial.println("\n>>> Client Connected <<<");
    
    String currentLine = "";
    String request = "";
    unsigned long timeout = millis() + 1000; // 1 second timeout
    
    while (client.connected() && millis() < timeout) {
      if (client.available()) {
        char c = client.read();
        
        if (c == '\n') {
          // Empty line means end of HTTP headers
          if (currentLine.length() == 0) {
            // Extract command from request
            String cmd = "";
            if (request.startsWith("GET /")) {
              int start = 5;
              int end = request.indexOf(' ', start);
              if (end > start) {
                cmd = request.substring(start, end);
              }
            }
            
            cmd.toUpperCase();
            Serial.print("Command: ");
            Serial.println(cmd);
            
            // Send HTTP response immediately
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");
            client.println();
            
            // Process command and send response
            processCommand(cmd, client);
            
            client.flush(); // Ensure data is sent
            break;
          } else {
            // Save the first line (GET request)
            if (request.length() == 0) {
              request = currentLine;
            }
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    
    client.stop();
    Serial.println(">>> Client Disconnected <<<");
  }
}

void processCommand(String cmd, WiFiClient &client) {
  // Motor 1 commands
  if (cmd == "1F") {
    motor1Forward();
    client.print("<html><body><h2>Motor 1 Forward</h2><a href='/'>Back</a></body></html>");
  }
  else if (cmd == "1B") {
    motor1Backward();
    client.print("<html><body><h2>Motor 1 Backward</h2><a href='/'>Back</a></body></html>");
  }
  else if (cmd == "1S") {
    motor1Stop();
    client.print("<html><body><h2>Motor 1 Stopped</h2><a href='/'>Back</a></body></html>");
  }
  
  // Motor 2 commands
  else if (cmd == "2F") {
    motor2Forward();
    client.print("<html><body><h2>Motor 2 Forward</h2><a href='/'>Back</a></body></html>");
  }
  else if (cmd == "2B") {
    motor2Backward();
    client.print("<html><body><h2>Motor 2 Backward</h2><a href='/'>Back</a></body></html>");
  }
  else if (cmd == "2S") {
    motor2Stop();
    client.print("<html><body><h2>Motor 2 Stopped</h2><a href='/'>Back</a></body></html>");
  }
  
  // All motors commands
  else if (cmd == "AF") {
    allForward();
    client.print("<html><body><h2>All Motors Forward</h2><a href='/'>Back</a></body></html>");
  }
  else if (cmd == "AS") {
    allStop();
    client.print("<html><body><h2>All Motors Stopped</h2><a href='/'>Back</a></body></html>");
  }
  
  // Home page
  else {
    client.print("<html><head><style>");
    client.print("body{font-family:Arial;text-align:center;margin-top:50px;}");
    client.print("a{display:inline-block;background:#4CAF50;color:white;padding:15px 30px;");
    client.print("text-decoration:none;margin:10px;border-radius:5px;font-size:18px;}");
    client.print("a:hover{background:#45a049;}");
    client.print(".stop{background:#f44336;} .stop:hover{background:#da190b;}");
    client.print("</style></head><body>");
    client.print("<h1>Conveyor Control</h1>");
    client.print("<div><a href='/1F'>Motor 1 ▲</a><a href='/1B'>Motor 1 ▼</a><a href='/1S' class='stop'>Motor 1 ■</a></div>");
    client.print("<div><a href='/2F'>Motor 2 ▲</a><a href='/2B'>Motor 2 ▼</a><a href='/2S' class='stop'>Motor 2 ■</a></div>");
    client.print("<div><a href='/AF'>All Forward ▲▲</a><a href='/AS' class='stop'>STOP ALL ■■</a></div>");
    client.print("</body></html>");
  }
}
