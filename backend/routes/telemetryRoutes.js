const express = require('express');
const router = express.Router();
const telemetryController = require('../controllers/telemetryController');

// GET /api/telemetry/latest?n=50
router.get('/latest', (req, res) => {
  const n = parseInt(req.query.n || '100', 10);
  const rows = telemetryController.getLatest(n);
  res.json({ count: rows.length, data: rows });
});

module.exports = router;
