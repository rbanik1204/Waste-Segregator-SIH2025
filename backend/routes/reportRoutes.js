const express = require('express');
const router = express.Router();
const reportController = require('../controllers/reportController');

// GET /api/reports/csv
router.get('/csv', (req, res) => {
  try {
    const csv = reportController.generateCSV();
    res.header('Content-Type', 'text/csv');
    res.attachment('pavitrax_telemetry.csv');
    return res.send(csv);
  } catch (err) {
    console.error(err);
    res.status(500).json({ error: 'CSV generation failed' });
  }
});

module.exports = router;
