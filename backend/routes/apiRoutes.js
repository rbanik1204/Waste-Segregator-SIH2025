const express = require('express');
const sensorController = require('../controllers/sensorController');
const statusController = require('../controllers/statusController');
const classificationController = require('../controllers/classificationController');
const analyticsController = require('../controllers/analyticsController');
const exportController = require('../controllers/exportController');
const alertController = require('../controllers/alertController');
const telemetryController = require('../controllers/telemetryController');

const router = express.Router();

// Sensors ingest
router.post('/sensors/upload', sensorController.upload);

// Boat control/status
router.get('/boat/status', statusController.getStatus);

// Waste classification
router.post('/waste/classification', classificationController.classify);

// Exports
router.get('/export/csv', exportController.exportCsv);
router.get('/export/pdf', exportController.exportPdf);

// Analytics
router.get('/analytics/weekly', analyticsController.weekly);
router.get('/analytics/classification', analyticsController.classificationStats);
router.get('/analytics/heatmap', async (req, res) => {
  const points = await telemetryController.getHeatmapPoints(800);
  res.json({ ok: true, data: points });
});

// Alerts
router.get('/alerts', alertController.list);
router.post('/alerts/:id/ack', alertController.acknowledge);

// Latest telemetry (mirrors legacy route but under /api)
router.get('/sensors/latest', (req, res) => {
  const n = parseInt(req.query.n || '100', 10);
  const rows = telemetryController.getLatest(n);
  res.json({ count: rows.length, data: rows });
});

module.exports = router;

