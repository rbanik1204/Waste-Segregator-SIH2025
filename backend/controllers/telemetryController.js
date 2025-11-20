// telemetryController.js
// simple in-memory telemetry store for demo — replace with DB if needed
const maxStore = 5000;
const store = []; // { timestamp, topic, data, receivedAt }

const mqttBridge = require('../mqtt');

function addTelemetry(item) {
  store.push(item);
  if (store.length > maxStore) store.shift();
}

function getLatest(n = 100) {
  return store.slice(-Math.min(n, store.length)).reverse();
}

function clearStore() {
  store.length = 0;
}

// listen to mqtt events and store
mqttBridge.on('telemetry', (payload) => {
  const item = {
    timestamp: new Date().toISOString(),
    topic: payload.topic,
    data: payload.data,
    receivedAt: payload.receivedAt || new Date().toISOString()
  };
  addTelemetry(item);
});

module.exports = {
  addTelemetry,
  getLatest,
  clearStore,
  // expose store for CSV generation
  _store: store
};
