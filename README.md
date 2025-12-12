# Waste Segregator - Smart Waste Management System

## 🎯 Problem Statement & Solution

### Problem
Traditional waste management systems lack real-time monitoring, automated segregation, and intelligent analytics, leading to inefficient waste processing and environmental concerns.

### Solution
An IoT-enabled smart waste segregation system that combines:
- **Real-time monitoring** using ESP8266/ESP32 microcontrollers
- **AI-powered waste classification** using YOLOv8 and CNN models
- **Automated conveyor control** for waste sorting
- **Live analytics dashboard** for monitoring and reporting
- **MQTT-based telemetry** for sensor data collection

---

## ✨ Features

### Hardware Integration
- ESP8266 sensor integration (ultrasonic, gas sensors, proximity sensors)
- ESP32-CAM for real-time image capture and processing
- Dual conveyor belt control system
- LJ12A3 proximity sensor support
- Real-time telemetry data collection via MQTT

### AI/ML Capabilities
- **YOLOv8 Object Detection** for waste classification
- **CNN-based Classification** with GradCAM visualization
- Real-time inference on webcam/camera feeds
- Custom waste dataset training support
- Heatmap generation for waste distribution analysis

### Backend Services
- Node.js REST API server
- WebSocket support for real-time updates
- MQTT broker integration (Mosquitto)
- MongoDB database for telemetry storage
- CSV data export functionality
- Automated daily analytics and forecasting

### Frontend Dashboard
- Real-time sensor monitoring
- Live waste classification display
- Interactive analytics charts
- Control interface for conveyor systems
- Alert management system
- PDF report generation

---

## 🛠️ Technologies Used

### Hardware
- ESP8266 (NodeMCU)
- ESP32-CAM
- Ultrasonic Sensors (HC-SR04)
- Gas Sensors (MQ series)
- Proximity Sensors (LJ12A3)
- Servo Motors & Conveyors

### Software Stack

#### Backend
- **Runtime:** Node.js v16+
- **Framework:** Express.js
- **Database:** MongoDB
- **Message Broker:** MQTT (Mosquitto)
- **Real-time:** WebSocket (ws)

#### AI/ML
- **Language:** Python 3.8+
- **Frameworks:** 
  - PyTorch
  - Ultralytics YOLOv8
  - TensorFlow/Keras
  - FastAPI
- **Libraries:**
  - OpenCV (cv2)
  - NumPy, Pandas
  - Matplotlib, Seaborn
  - scikit-learn

#### Frontend
- HTML5, CSS3, JavaScript (Vanilla)
- Chart.js for data visualization
- WebSocket client for real-time updates

---

## 📋 Prerequisites

### System Requirements
- **OS:** Windows 10/11, Linux, or macOS
- **RAM:** Minimum 4GB (8GB recommended)
- **Storage:** 2GB free space

### Software Requirements
- Node.js v16+ and npm
- Python 3.8+ and pip
- MongoDB v4.4+ (optional for full features)
- Mosquitto MQTT Broker (optional for IoT features)
- Arduino IDE (for microcontroller programming)

### Hardware Requirements (Optional)
- ESP8266/ESP32 boards
- Compatible sensors (ultrasonic, gas, proximity)
- USB cables for programming
- Conveyor belt setup (for full deployment)

---

## 🚀 Installation & Setup

### Step 1: Extract the Project
Extract the project folder to your desired location.

### Step 2: Python/AI Setup (Core System)

#### Create Virtual Environment
```powershell
# Windows
python -m venv venv
.\venv\Scripts\Activate.ps1

# Linux/Mac
python3 -m venv venv
source venv/bin/activate
```

#### Install Python Dependencies
```powershell
pip install --upgrade pip
pip install -r backend/ai/docker/requirements.txt
```

If you encounter issues with PyTorch, install CPU version:
```powershell
pip install torch --index-url https://download.pytorch.org/whl/cpu
pip install ultralytics
```

#### Verify Model File
Ensure YOLOv8 model is present at: `backend/ai/yolo/yolov8n.pt`

### Step 3: Backend Setup (Optional - for full features)

#### Install Node.js Dependencies
```powershell
cd backend
npm install
```

#### Install & Start MongoDB (Optional)
- Download from [MongoDB Official Site](https://www.mongodb.com/try/download/community)
- Start MongoDB service:
  ```powershell
  # Windows
  net start MongoDB
  ```

#### Install Mosquitto MQTT Broker (Optional)
See [backend/INSTALL_MOSQUITTO.md](backend/INSTALL_MOSQUITTO.md) for detailed instructions.

### Step 4: Arduino/Microcontroller Setup (Optional)

For hardware integration:
1. Install Arduino IDE from [Arduino Official Site](https://www.arduino.cc/en/software)
2. Install ESP8266/ESP32 board support
3. See [backend/ARDUINO_SETUP.md](backend/ARDUINO_SETUP.md) for complete guide
4. Upload firmware from:
   - ESP8266: `backend/ESP8266/esp8266_telemetry/`
   - ESP32: `backend/ESP32/esp32cam_telemetry.ino`

---

## 🎮 Running the Project

### Quick Start (AI System Only)

#### 1. Start the AI API Server
```powershell
python backend/ai/api/main.py
```
Server runs on: `http://127.0.0.1:8000`

#### 2. Run Webcam Detection Demo
In a new terminal:
```powershell
python backend/ai/yolo/post_webcam_demo.py
```
This captures webcam images and performs real-time waste detection.

#### 3. View Results
- **CSV Data:** `http://127.0.0.1:8000/csv`
- **Heatmap:** Run `python backend/ai/yolo/generate_heatmap.py` after collecting data

### Full System (with Backend & Frontend)

#### 1. Start MongoDB (if installed)
```powershell
net start MongoDB
```

#### 2. Start Mosquitto MQTT Broker (if installed)
```powershell
net start mosquitto
```

#### 3. Start Backend Server
```powershell
cd backend
node server.js
```
Server runs on: `http://localhost:3000`

#### 4. Start AI API
```powershell
python backend/ai/api/main.py
```

#### 5. Open Frontend Dashboard
Open `frontend/index.html` in a web browser or use:
```powershell
# Using Python
python -m http.server 8080
# Then navigate to: http://localhost:8080
```

#### 6. Flash ESP8266/ESP32 (Optional - for hardware integration)
Upload firmware from `backend/ESP8266/esp8266_telemetry/` or `backend/ESP32/esp32cam_telemetry.ino`
- Update WiFi credentials (SSID, password)
- Update server IP address
- ESP will GET `/detected` and POST `/telemetry`

---

## 🔧 Configuration

### Environment Variables

Create a `.env` file in the `backend/` directory (optional):

```env
# MongoDB Configuration
MONGO_URI=mongodb://localhost:27017/waste_segregator
DB_NAME=waste_segregator

# MQTT Configuration
MQTT_BROKER=mqtt://localhost:1883
MQTT_TOPIC_TELEMETRY=waste/telemetry
MQTT_TOPIC_CONTROL=waste/control

# Server Configuration
PORT=3000
WEBSOCKET_PORT=8080

# AI API Configuration
AI_API_URL=http://localhost:8000
YOLO_MODEL_PATH=./backend/ai/yolo/yolov8n.pt
```

### Model Path Configuration
The system looks for YOLOv8 model at:
1. `./backend/ai/yolo/yolov8n.pt` (default)
2. `/mnt/data/yolov8n.pt` (alternate)

---

## 📊 Features Guide

### Real-time Monitoring
- View live sensor data (ultrasonic distance, gas levels)
- Monitor conveyor belt status
- Track waste classification results

### AI Waste Classification
- **YOLOv8 Detection:** Real-time object detection with bounding boxes
- **CNN Classification:** Deep learning classification with confidence scores
- **GradCAM:** Visual explanations for CNN predictions
- **Heatmap Generation:** Waste distribution analysis

### Control System
- Start/stop conveyor belts remotely
- Configure sensor thresholds
- Manual override controls
- Alert management

### Analytics & Reporting
After collecting telemetry data, generate reports:

```powershell
# Generate heatmap
python backend/ai/yolo/generate_heatmap.py

# Generate PDF report
python backend/ai/reports/generate_pdf_report.py
```

Output files:
- Heatmap: `backend/data/heatmap.html`
- Report: `backend/data/report_latest.pdf`
- CSV: Available at `http://127.0.0.1:8000/csv`

---

## 📁 Project Structure

```
Waste Segregator/
├── backend/
│   ├── ai/                    # AI/ML modules
│   │   ├── analytics/         # Data analytics & forecasting
│   │   ├── api/              # FastAPI endpoints
│   │   ├── cnn/              # CNN classification
│   │   ├── reports/          # PDF report generation
│   │   └── yolo/             # YOLOv8 detection
│   ├── config/               # Configuration files
│   ├── controllers/          # API controllers
│   ├── data/                 # Telemetry CSV & images
│   ├── ESP8266/              # ESP8266 firmware
│   ├── ESP32/                # ESP32-CAM firmware
│   ├── models/               # MongoDB schemas
│   ├── routes/               # API routes
│   ├── utils/                # Utility functions
│   ├── server.js             # Main Node.js server
│   └── package.json          # Node.js dependencies
├── frontend/
│   ├── index.html            # Dashboard UI
│   ├── app.js                # Frontend logic
│   └── app.css               # Styles
├── README.md                 # This file
└── team_info.txt             # Team information
```

---

## 🧪 Testing

### Test AI API Server
```powershell
# Check server status
curl http://localhost:8000

# Download CSV data
curl http://localhost:8000/csv
```

### Test Backend Server
```powershell
cd backend
node test-server.js
```

### Test MQTT Connection (if installed)
```powershell
# Subscribe to telemetry
mosquitto_sub -t "waste/telemetry"

# Publish test message
mosquitto_pub -t "waste/telemetry" -m '{"sensorId":"test","distance":50}'
```

### Test AI Endpoints
```powershell
# YOLOv8 inference
curl -X POST http://localhost:8000/yolo/detect -F "file=@test_image.jpg"

# CNN classification
curl -X POST http://localhost:8000/cnn/classify -F "file=@test_image.jpg"
```

---

## 📸 Sample Data & Outputs

### Sample Sensor Data (MQTT)
```json
{
  "sensorId": "ESP8266_01",
  "timestamp": "2025-12-12T10:30:00Z",
  "distance": 45.2,
  "gasLevel": 120,
  "temperature": 28.5,
  "detectedClass": "plastic_bottle",
  "status": "operational"
}
```

### Sample API Response (YOLO)
```json
{
  "detections": [
    {
      "class": "plastic_bottle",
      "confidence": 0.94,
      "bbox": [120, 80, 250, 300]
    }
  ],
  "processing_time": 0.23,
  "timestamp": "2025-12-12T10:30:15Z"
}
```

---

## 🐛 Troubleshooting

### Common Issues

**Python/AI Server Issues:**
- **Import Error:** Ensure all dependencies are installed: `pip install -r backend/ai/docker/requirements.txt`
- **Model Not Found:** Verify `yolov8n.pt` exists at `backend/ai/yolo/yolov8n.pt`
- **PyTorch Error:** Install CPU version: `pip install torch --index-url https://download.pytorch.org/whl/cpu`

**Backend Server Issues:**
- **MongoDB Connection Error:** Ensure MongoDB service is running or skip MongoDB features
- **Port Already in Use:** Change port in server configuration

**MQTT Connection Issues:**
- **Connection Failed:** Verify Mosquitto broker is running
- **Firewall Block:** Allow port 1883 in Windows Firewall

**ESP8266/ESP32 Issues:**
- **Not Connecting:** Verify WiFi credentials and server IP
- **Upload Failed:** Check board selection and COM port
- **No Data:** Ensure server is running and accessible

**Windows Firewall:**
- When prompted, allow Python to accept incoming connections
- If missed: Windows Defender Firewall → Allow an app → Add `python.exe` from venv

---

## 📚 Additional Documentation

- [Arduino Setup Guide](backend/ARDUINO_SETUP.md)
- [Mosquitto Installation](backend/INSTALL_MOSQUITTO.md)
- [YOLOv8 Training Guide](backend/ai/yolo/TRAINING_GUIDE.md)
- [Webcam Demo Guide](backend/ai/yolo/README_WEBCAM.md)
- [Utils Documentation](backend/utils/README.md)

---

## 💡 Tips for Demo

- **Local Demo:** Run all services on your laptop
- **Remote Demo:** Use `ngrok` to expose the server for remote access
  ```powershell
  ngrok http 8000
  ```
- **Data Collection:** Run webcam demo for a few minutes to collect data before generating reports
- **Test Before Demo:** Verify all services are running and accessible

---

## 🤝 Contributing

This is a Smart India Hackathon 2025 project. For team member contributions, please coordinate with the team lead.

---

## 📄 License

See [LICENSE](LICENSE) file for details.

---

## 🙏 Acknowledgments

- Smart India Hackathon 2025
- Ultralytics YOLOv8 Framework
- Arduino & ESP Community
- Open-source contributors

---

## 📞 Support

For issues or queries, contact the team members listed in [team_info.txt](team_info.txt).

---

**Note:** This project is developed for Smart India Hackathon 2025. All rights reserved by the team members.
