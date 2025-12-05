const BoatStatus = require('../models/BoatStatus');

async function getStatus(req, res) {
  const status = await BoatStatus.findOne().sort({ updatedAt: -1 }).lean();
  res.json({ ok: true, data: status });
}

module.exports = { getStatus };

