// telemetryController.js
// Stores telemetry into MongoDB while keeping a small in-memory buffer for fast UI.
const Telemetry = require('../models/Telemetry');
const { aggregateForHeatmap } = require('../utils/heatmapHelper');
const alertController = require('./alertController');

const maxStore = 5000;
const store = []; // { timestamp, topic, data, receivedAt }

const mqttBridge = require('../mqtt');

function addTelemetry(item) {
  store.push(item);
  if (store.length > maxStore) store.shift();
}

async function ingest(payload, source = 'mqtt') {
  const doc = new Telemetry({
    timestamp: payload.timestamp || new Date(),
    gps: {
      lat: payload.data?.lat || payload.data?.latitude,
      lon: payload.data?.lon || payload.data?.longitude
    },
    heading: payload.data?.heading || payload.data?.heading_deg,
    speed: payload.data?.speed,
    ultrasonic_cm: payload.data?.ultrasonic_cm || payload.data?.ultrasonic,
    accelerometer: payload.data?.accelerometer || payload.data?.adx || payload.data?.accel,
    temperature_c: payload.data?.temperature || payload.data?.temp_c || payload.data?.temp,
    tds_ppm: payload.data?.tds || payload.data?.tds_ppm, // Total Dissolved Solids
    gas: {
      mq2: payload.data?.mq2 || payload.data?.mq2_ppm,
      mq135: payload.data?.mq135 || payload.data?.mq135_ppm
    },
    moisture: {
      dry_bin: payload.data?.moisture_dry,
      wet_bin: payload.data?.moisture_wet
    },
    loadcell_grams: payload.data?.loadcell || payload.data?.weight,
    battery_volt: payload.data?.battery || payload.data?.battery_volt,
    conveyor: {
      wet_active: payload.data?.conveyor_wet,
      dry_active: payload.data?.conveyor_dry
    },
    mode: {
      autonomous: payload.data?.autonomous,
      navigation: payload.data?.navigation
    },
    waste: payload.data?.waste,
    raw: payload.data,
    source
  });

  try {
    await doc.save();
    evaluateAlerts(doc);
  } catch (err) {
    console.error('Failed to save telemetry', err.message);
  }

  const item = {
    timestamp: doc.timestamp.toISOString(),
    topic: payload.topic,
    data: payload.data,
    receivedAt: payload.receivedAt || new Date().toISOString()
  };
  addTelemetry(item);
  return doc;
}

function getLatest(n = 100) {
  return store.slice(-Math.min(n, store.length)).reverse();
}

function clearStore() {
  store.length = 0;
}

async function getWeeklySummary() {
  const since = new Date();
  since.setDate(since.getDate() - 7);
  const pipeline = [
    { $match: { timestamp: { $gte: since } } },
    {
      $group: {
        _id: {
          day: { $dateToString: { format: '%Y-%m-%d', date: '$timestamp' } }
        },
        totalWeight: { $sum: '$loadcell_grams' },
        avgGas: { $avg: '$gas.mq135' },
        maxTemp: { $max: '$temperature_c' },
        count: { $sum: 1 }
      }
    },
    { $sort: { '_id.day': 1 } }
  ];
  const result = await Telemetry.aggregate(pipeline);
  return result;
}

async function getHeatmapPoints(limit = 500) {
  const docs = await Telemetry.find({ 'gps.lat': { $ne: null }, 'gps.lon': { $ne: null } })
    .sort({ timestamp: -1 })
    .limit(limit)
    .lean();
  return aggregateForHeatmap(
    docs.map((d) => ({
      data: {
        latitude: d.gps.lat,
        longitude: d.gps.lon,
        weight: d.waste?.type === 'hazardous' ? 3 : d.waste?.type === 'wet' ? 2 : 1
      }
    }))
  );
}

// listen to mqtt events and store
mqttBridge.on('telemetry', async (payload) => {
  try {
    await ingest(payload, 'mqtt');
  } catch (err) {
    console.error('MQTT ingest error', err);
  }
});

function evaluateAlerts(doc) {
  const alerts = [];
  if ((doc.loadcell_grams || 0) > 4500) alerts.push({ type: 'bin_full', message: 'Bin weight above threshold', severity: 'critical' });
  if ((doc.battery_volt || 0) < 10.8) alerts.push({ type: 'low_battery', message: `Battery low: ${doc.battery_volt}V`, severity: 'warning' });
  if ((doc.ultrasonic_cm || 999) < 25) alerts.push({ type: 'obstacle', message: `Obstacle detected at ${doc.ultrasonic_cm}cm`, severity: 'critical' });
  if ((doc.gas?.mq135 || 0) > 400) alerts.push({ type: 'pollution', message: `High MQ135 reading ${doc.gas.mq135}`, severity: 'warning' });
  const acc = doc.accelerometer || {};
  if (Math.abs(acc.x || 0) > 1.2 || Math.abs(acc.y || 0) > 1.2) alerts.push({ type: 'tilt', message: 'Boat tilt exceeds threshold', severity: 'critical' });
  if (doc.waste?.type && String(doc.waste.type).toLowerCase().includes('hazard')) {
    alerts.push({ type: 'hazardous_waste', message: 'Hazardous waste detected', severity: 'critical' });
  }
  alerts.forEach((a) => alertController.createAlert(a.type, a.message, { telemetryId: doc._id }, a.severity).catch(() => {}));
}

module.exports = {
  addTelemetry,
  ingest,
  getLatest,
  clearStore,
  getWeeklySummary,
  getHeatmapPoints,
  // expose store for CSV generation
  _store: store
};
