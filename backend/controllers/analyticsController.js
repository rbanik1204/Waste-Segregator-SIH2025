const Telemetry = require('../models/Telemetry');
const dayjs = require('dayjs');

async function weekly(req, res) {
  try {
    const since = dayjs().subtract(7, 'day').toDate();
    const docs = await Telemetry.aggregate([
      { $match: { timestamp: { $gte: since } } },
      {
        $group: {
          _id: { day: { $dateToString: { format: '%Y-%m-%d', date: '$timestamp' } } },
          totalWeight: { $sum: '$loadcell_grams' },
          avgGas: { $avg: '$gas.mq135' },
          avgTemp: { $avg: '$temperature_c' },
          wet: { $sum: { $cond: [{ $eq: ['$waste.type', 'wet'] }, 1, 0] } },
          dry: { $sum: { $cond: [{ $eq: ['$waste.type', 'dry'] }, 1, 0] } },
          hazardous: { $sum: { $cond: [{ $eq: ['$waste.type', 'hazardous'] }, 1, 0] } }
        }
      },
      { $sort: { '_id.day': 1 } }
    ]);
    res.json({ ok: true, data: docs });
  } catch (err) {
    console.error(err);
    res.status(500).json({ error: 'Weekly analytics failed' });
  }
}

async function classificationStats(req, res) {
  try {
    const since = dayjs().subtract(1, 'day').toDate();
    const docs = await Telemetry.aggregate([
      { $match: { timestamp: { $gte: since } } },
      {
        $group: {
          _id: '$waste.type',
          count: { $sum: 1 },
          avgConfidence: { $avg: '$waste.confidence' }
        }
      }
    ]);
    res.json({ ok: true, data: docs });
  } catch (err) {
    console.error(err);
    res.status(500).json({ error: 'Classification stats failed' });
  }
}

module.exports = { weekly, classificationStats };

