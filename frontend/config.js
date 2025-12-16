// Configuration for PAVITRAX Frontend
// This file allows easy configuration of backend and hardware endpoints

// Determine if running on GitHub Pages or locally
const isGitHubPages = window.location.hostname.includes('github.io');

const CONFIG = {
  // Backend API Configuration
  // When running locally on port 3000, API runs on port 4000
  // When on GitHub Pages, you'll need to deploy backend separately and update this URL
  API_BASE: location.origin.includes('3000')
    ? location.origin.replace('3000', '4000')
    : location.origin,

  // YOLO Stream Configuration
  // For local development, this points to local YOLO stream server
  // For GitHub Pages deployment, update this to your deployed YOLO stream URL
  YOLO_STREAM_URL: isGitHubPages 
    ? 'https://your-yolo-server.example.com/stream' 
    : 'http://127.0.0.1:8090/stream',

  // ESP8266/Hardware Configuration
  // These are for direct hardware control (will only work on local network)
  ESP8266_IP: '192.168.4.1',
  ESP8266_PORT: '80',

  // Feature Flags
  FEATURES: {
    // Disable hardware features when on GitHub Pages (since hardware won't be accessible)
    ENABLE_HARDWARE_CONTROL: !isGitHubPages,
    ENABLE_YOLO_STREAM: true,
    ENABLE_CAMERA: true
  }
};

// Make configuration available globally
window.APP_CONFIG = CONFIG;
