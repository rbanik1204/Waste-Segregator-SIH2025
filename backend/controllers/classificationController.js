const axios = require('axios');
const telemetryController = require('./telemetryController');
const BoatStatus = require('../models/BoatStatus');

const CLASSIFIER_BASE = process.env.CLASSIFIER_URL_BASE || 'http://localhost:8000';

async function classify(req, res) {
  try {
    const { imageUrl, sensors = {}, imageBase64 } = req.body || {};
    if (!imageUrl && !imageBase64) return res.status(400).json({ error: 'imageUrl or imageBase64 required' });

    // Call FastAPI classifier (URL or base64). /image_url and /image_base64 exist in FastAPI.
    const endpoint = imageUrl ? `${CLASSIFIER_BASE}/image_url` : `${CLASSIFIER_BASE}/image_base64`;
    const payload = imageUrl ? { imageUrl, sensors } : { imageBase64, sensors };
    const result = await axios.post(endpoint, payload, { timeout: 30000 });
    const data = result.data || {};
    // store telemetry event for classification
    await telemetryController.ingest(
      {
        topic: 'classification',
        data: {
          waste: {
            type: data.waste_category || data.type,
            confidence: data.confidence,
            bbox: data.bounding_box
          },
          lat: sensors.lat,
          lon: sensors.lon,
          loadcell: sensors.loadcell
        }
      },
      'classifier'
    );
    // simple bin selection logic
    const bin = decideBin(data.waste_category || data.type);
    await BoatStatus.findOneAndUpdate(
      { boatId: sensors.boat_id || 'boat-1' },
      { conveyors: { wet: bin === 'wet', dry: bin === 'dry' }, updatedAt: new Date() },
      { upsert: true }
    );
    return res.json({ ok: true, bin, result: data });
  } catch (err) {
    console.error(err);
    res.status(500).json({ error: 'Classification failed' });
  }
}

function decideBin(type) {
  const t = (type || '').toLowerCase();
  if (t.includes('wet') || t.includes('organic')) return 'wet';
  if (t.includes('hazard')) return 'hazard';
  if (t.includes('metal')) return 'metal';
  return 'dry';
}

module.exports = { classify };

