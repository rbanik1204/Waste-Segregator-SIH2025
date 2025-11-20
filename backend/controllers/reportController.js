const telemetryController = require('./telemetryController');
const { parse } = require('json2csv');

// create CSV from telemetry store
function generateCSV(options = {}) {
  // flatten store entries into rows
  const rows = telemetryController._store.map((s) => {
    return {
      timestamp: s.timestamp,
      topic: s.topic,
      receivedAt: s.receivedAt,
      payload: JSON.stringify(s.data)
    };
  });

  const fields = ['timestamp','topic','receivedAt','payload'];
  const opts = { fields };
  const csv = parse(rows, opts);
  return csv;
}

module.exports = { generateCSV };
