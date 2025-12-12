# Dual Conveyor Belt Controller

ESP8266-based WiFi controller for two independent conveyor belts (wet/dry waste sorting).

## Hardware Setup

### Motor Driver Connections (L298N or similar)

**Conveyor 1 (Wet Waste):**
- `ENA` → D5 (PWM speed control)
- `IN1` → D1 (Direction control 1)
- `IN2` → D2 (Direction control 2)

**Conveyor 2 (Dry Waste):**
- `ENA` → D6 (PWM speed control)
- `IN1` → D3 (Direction control 1)
- `IN2` → D4 (Direction control 2)

### Power Supply
- Motor driver VCC: 12V DC (or motor voltage)
- Motor driver GND: Common ground with ESP8266
- ESP8266: 5V via USB or separate regulator

## WiFi Configuration

- **SSID:** PavitraX
- **Password:** 12345678
- **IP Address:** 192.168.4.1 (default AP mode)
- **TCP Port:** 80

## Commands

### Two-Character Command Format

Send 2-character commands to control conveyors:

**Conveyor 1:**
- `1F` - Forward
- `1B` - Backward
- `1S` - Stop

**Conveyor 2:**
- `2F` - Forward
- `2B` - Backward
- `2S` - Stop

**Both Conveyors:**
- `AF` - All Forward
- `AB` - All Backward
- `AS` - All Stop

### Testing with netcat/telnet

**Windows PowerShell:**
```powershell
# Connect to ESP8266
$client = New-Object System.Net.Sockets.TcpClient("192.168.4.1", 80)
$stream = $client.GetStream()
$writer = New-Object System.IO.StreamWriter($stream)

# Send commands
$writer.WriteLine("1F")  # Conveyor 1 forward
$writer.Flush()
Start-Sleep -Seconds 3

$writer.WriteLine("1S")  # Conveyor 1 stop
$writer.Flush()

$client.Close()
```

**Linux/Mac:**
```bash
# Using netcat
echo "1F" | nc 192.168.4.1 80
echo "2F" | nc 192.168.4.1 80
echo "AS" | nc 192.168.4.1 80
```

**Using telnet:**
```bash
telnet 192.168.4.1 80
# Then type: 1F [Enter]
```

## Features

✅ **Independent Control** - Each conveyor operates separately  
✅ **PWM Speed Control** - Adjustable speed (default 900/1023)  
✅ **Safety Stop** - Motors stop when client disconnects  
✅ **Command Validation** - Invalid commands rejected  
✅ **Client Timeout** - Auto-disconnect after 30s inactivity  
✅ **Serial Debugging** - All commands logged to Serial Monitor

## Configuration

Edit these constants in the code:

```cpp
// WiFi credentials
const char* WIFI_SSID = "PavitraX";
const char* WIFI_PASSWORD = "12345678";

// Motor speed (0-1023)
const int MOTOR_SPEED = 900;

// Client timeout (milliseconds)
const unsigned long TIMEOUT = 30000;
```

## Pin Mapping Reference

| ESP8266 | Function | L298N Pin |
|---------|----------|-----------|
| D1 | Conveyor 1 IN1 | IN1 |
| D2 | Conveyor 1 IN2 | IN2 |
| D5 | Conveyor 1 ENA | ENA (PWM) |
| D3 | Conveyor 2 IN1 | IN3 |
| D4 | Conveyor 2 IN2 | IN4 |
| D6 | Conveyor 2 ENA | ENB (PWM) |
| GND | Ground | GND |

## Troubleshooting

**Motors not moving:**
- Check power supply (12V to motor driver)
- Verify all GND connections
- Check motor wiring polarity
- Reduce `MOTOR_SPEED` if stalling (try 700)

**WiFi connection fails:**
- Verify SSID/password in code
- Check ESP8266 is in AP mode (blue LED)
- Ensure device is connected to "PavitraX" network

**Wrong direction:**
- Swap motor wires, OR
- Swap IN1/IN2 pins in code

**Speed too slow/fast:**
- Adjust `MOTOR_SPEED` constant (512-1023)
- Lower values = slower, higher = faster

## Integration with Waste Sorting System

This controller can be integrated with your YOLO detection system:

1. YOLO detects waste type (wet/dry)
2. Backend sends TCP command to ESP8266
3. Appropriate conveyor activates
4. Waste is sorted automatically

Example Node.js integration:
```javascript
const net = require('net');

function activateConveyor(conveyorNum, duration = 3000) {
  const client = net.connect(80, '192.168.4.1', () => {
    client.write(`${conveyorNum}F`); // Forward
    
    setTimeout(() => {
      client.write(`${conveyorNum}S`); // Stop
      client.end();
    }, duration);
  });
}

// Use with YOLO detection
if (wasteType === 'wet') {
  activateConveyor(1); // Conveyor 1 for wet waste
} else if (wasteType === 'dry') {
  activateConveyor(2); // Conveyor 2 for dry waste
}
```
