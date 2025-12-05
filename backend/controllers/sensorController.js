const telemetryController = require('./telemetryController');
const BoatStatus = require('../models/BoatStatus');

// POST /api/sensors/upload
async function upload(req, res) {
  try {
    const data = req.body || {};
    const payload = {
      topic: 'api/telemetry',
      data,
      receivedAt: new Date().toISOString()
    };
    const doc = await telemetryController.ingest(payload, 'api');
    // optionally update boat status battery/mode
    if (data.battery || data.battery_volt || data.mode) {
      await BoatStatus.findOneAndUpdate(
        { boatId: data.boat_id || 'boat-1' },
        {
          battery_volt: data.battery_volt || data.battery,
          mode: data.mode || {},
          lastCommand: data.lastCommand || null,
          updatedAt: new Date()
        },
        { upsert: true, new: true }
      );
    }
    return res.json({ ok: true, id: doc._id });
  } catch (err) {
    console.error(err);
    return res.status(500).json({ error: 'Failed to store telemetry' });
  }
}

module.exports = { upload };

