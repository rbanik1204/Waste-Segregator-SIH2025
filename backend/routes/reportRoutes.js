const express = require('express');
const router = express.Router();
const exportController = require('../controllers/exportController');

// GET /api/reports/csv
router.get('/csv', (req, res) => {
  try {
    return exportController.exportCsv(req, res);
  } catch (err) {
    console.error(err);
    res.status(500).json({ error: 'CSV generation failed' });
  }
});

module.exports = router;
