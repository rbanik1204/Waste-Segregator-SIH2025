const mongoose = require('mongoose');

const TelemetrySchema = new mongoose.Schema({
  timestamp: { type: Date, default: Date.now },
  gps: {
    lat: Number,
    lon: Number
  },
  heading: Number,
  speed: Number,
  ultrasonic_cm: Number,
  accelerometer: {
    x: Number,
    y: Number,
    z: Number
  },
  temperature_c: Number,
  gas: {
    mq2: Number,
    mq135: Number
  },
  moisture: {
    dry_bin: Number,
    wet_bin: Number
  },
  loadcell_grams: Number,
  battery_volt: Number,
  conveyor: {
    wet_active: Boolean,
    dry_active: Boolean
  },
  mode: {
    autonomous: Boolean,
    navigation: Boolean
  },
  waste: {
    type: String,
    confidence: Number,
    bbox: [Number],
    bin: String
  },
  raw: mongoose.Schema.Types.Mixed,
  source: { type: String, default: 'mqtt' }
}, { timestamps: true });

module.exports = mongoose.model('Telemetry', TelemetrySchema);

