const express = require('express');
const router = express.Router();
const controlController = require('../controllers/controlController');

// POST /api/control/send  { command: { action: "forward", speed: 50 } }
router.post('/send', (req, res) => {
  const command = req.body;
  if (!command) return res.status(400).json({ error: 'Missing command in body' });
  const result = controlController.sendCommand(command);
  res.json(result);
});

module.exports = router;
