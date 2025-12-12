const express = require('express');
const router = express.Router();

// Store latest gas sensor data in memory
let latestGasData = {
  mq135: { ppm: 0, gas: 'clean_air', level: 'good', hasError: false },
  mq2: { ppm: 0, gas: 'clean_air', level: 'none', hasError: false },
  timestamp: Date.now(),
  lastUpdate: null
};

// POST endpoint to receive data from ESP8266
router.post('/', (req, res) => {
  try {
    const { mq135, mq2, timestamp } = req.body;
    
    if (!mq135 || !mq2) {
      return res.status(400).json({ 
        success: false, 
        error: 'Missing mq135 or mq2 data' 
      });
    }
    
    latestGasData = {
      mq135,
      mq2,
      timestamp,
      lastUpdate: new Date().toISOString()
    };
    
    console.log('[Gas Sensors] Data received:', {
      mq135: `${mq135.ppm}ppm (${mq135.level})`,
      mq2: `${mq2.ppm}ppm (${mq2.level})`
    });
    
    res.json({ 
      success: true, 
      message: 'Gas sensor data received',
      data: latestGasData
    });
  } catch (error) {
    console.error('[Gas Sensors] Error:', error);
    res.status(500).json({ 
      success: false, 
      error: error.message 
    });
  }
});
// GET endpoint for base route (same as /latest)
router.get('/', (req, res) => {
  res.json({
    success: true,
    data: latestGasData
  });
});
// GET endpoint to retrieve latest data
router.get('/latest', (req, res) => {
  res.json({
    success: true,
    data: latestGasData
  });
});

module.exports = router;
