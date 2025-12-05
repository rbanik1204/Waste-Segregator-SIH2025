const mongoose = require('mongoose');

const AlertSchema = new mongoose.Schema({
  type: { type: String, required: true }, // bin_full, overload, pollution, hazard, tilt, low_battery, obstacle
  message: { type: String, required: true },
  severity: { type: String, default: 'warning' },
  payload: mongoose.Schema.Types.Mixed,
  acknowledged: { type: Boolean, default: false },
  createdAt: { type: Date, default: Date.now }
});

module.exports = mongoose.model('Alert', AlertSchema);

