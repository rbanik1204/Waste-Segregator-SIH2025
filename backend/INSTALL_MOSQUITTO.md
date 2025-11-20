# Installing Mosquitto MQTT Broker on Windows

## Option 1: Direct Download (Recommended)

1. **Download Mosquitto:**
   - Visit: https://mosquitto.org/download/
   - Download the Windows installer (e.g., `mosquitto-2.x.x-install-windows-x64.exe`)

2. **Install:**
   - Run the installer
   - During installation, check "Install as Windows Service" to run automatically
   - Default port: **1883**

3. **Verify Installation:**
   - Open PowerShell as Administrator
   - Run: `mosquitto -v`
   - Or check Windows Services: `Get-Service mosquitto`

## Option 2: Using Chocolatey (Package Manager)

### Step 1: Install Chocolatey
Open PowerShell as Administrator and run:
```powershell
Set-ExecutionPolicy Bypass -Scope Process -Force; [System.Net.ServicePointManager]::SecurityProtocol = [System.Net.ServicePointManager]::SecurityProtocol -bor 3072; iex ((New-Object System.Net.WebClient).DownloadString('https://community.chocolatey.org/install.ps1'))
```

### Step 2: Install Mosquitto
```powershell
choco install mosquitto
```

## Option 3: Using Docker (If you have Docker installed)

```powershell
docker run -it -p 1883:1883 eclipse-mosquitto
```

## Verify MQTT Connection

After installation, your backend will automatically connect to `mqtt://localhost:1883` (as configured in `config/mqttConfig.js`).

Test the connection by running your backend:
```powershell
npm start
```

You should see: `Connected to MQTT broker: mqtt://localhost:1883`

## Troubleshooting

- **Port already in use:** Another MQTT broker might be running. Check with: `netstat -ano | findstr :1883`
- **Service not starting:** Run PowerShell as Administrator and check: `Get-Service mosquitto | Start-Service`
- **Firewall:** Allow port 1883 through Windows Firewall if needed

