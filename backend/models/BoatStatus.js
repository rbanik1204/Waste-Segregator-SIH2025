const mongoose = require('mongoose');

const BoatStatusSchema = new mongoose.Schema({
  boatId: { type: String, default: 'boat-1' },
  mode: {
    autonomous: { type: Boolean, default: false },
    navigation: { type: Boolean, default: false },
    manualOverride: { type: Boolean, default: false }
  },
  conveyors: {
    wet: { type: Boolean, default: false },
    dry: { type: Boolean, default: false }
  },
  motors: {
    left: { type: Number, default: 0 },
    right: { type: Number, default: 0 }
  },
  battery_volt: Number,
  lastCommand: mongoose.Schema.Types.Mixed,
  updatedAt: { type: Date, default: Date.now }
});

module.exports = mongoose.model('BoatStatus', BoatStatusSchema);

