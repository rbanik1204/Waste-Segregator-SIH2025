const { parse } = require('json2csv');
const { PDFDocument, rgb, StandardFonts } = require('pdf-lib');
const Telemetry = require('../models/Telemetry');

async function exportCsv(req, res) {
  const rows = await Telemetry.find().sort({ timestamp: -1 }).limit(5000).lean();
  const fields = [
    'timestamp',
    'gps.lat',
    'gps.lon',
    'heading',
    'speed',
    'ultrasonic_cm',
    'temperature_c',
    'gas.mq2',
    'gas.mq135',
    'moisture.dry_bin',
    'moisture.wet_bin',
    'loadcell_grams',
    'battery_volt',
    'waste.type',
    'waste.confidence'
  ];
  const opts = { fields, defaultValue: '', flatten: true };
  const csv = parse(rows, opts);
  res.header('Content-Type', 'text/csv');
  res.attachment('pavitrax_telemetry.csv');
  return res.send(csv);
}

async function exportPdf(req, res) {
  const latest = await Telemetry.find().sort({ timestamp: -1 }).limit(20).lean();
  const doc = await PDFDocument.create();
  const page = doc.addPage([612, 792]); // Letter
  const font = await doc.embedFont(StandardFonts.Helvetica);
  const title = 'PAVITRAX Daily Report';
  page.drawText(title, { x: 50, y: 740, size: 20, font, color: rgb(0.1, 0.2, 0.5) });
  page.drawText(`Generated: ${new Date().toISOString()}`, { x: 50, y: 715, size: 10, font });
  let y = 690;
  const header = 'Time | Lat | Lon | Waste | Conf | Weight(g) | Gas(MQ135) | Battery(V)';
  page.drawText(header, { x: 50, y, size: 10, font, color: rgb(0, 0, 0) });
  y -= 15;
  latest.forEach((row) => {
    if (y < 60) return;
    const line = `${new Date(row.timestamp).toISOString()} | ${row.gps?.lat ?? '-'} | ${row.gps?.lon ?? '-'} | ${row.waste?.type ?? '-'} | ${(row.waste?.confidence ?? 0).toFixed(2)} | ${row.loadcell_grams ?? '-'} | ${row.gas?.mq135 ?? '-'} | ${row.battery_volt ?? '-'}`;
    page.drawText(line.slice(0, 110), { x: 50, y, size: 9, font });
    y -= 12;
  });
  const pdfBytes = await doc.save();
  res.header('Content-Type', 'application/pdf');
  res.attachment('pavitrax_report.pdf');
  return res.send(Buffer.from(pdfBytes));
}

module.exports = { exportCsv, exportPdf };

