const Alert = require('../models/Alert');
const nodemailer = require('nodemailer');

const mailUser = process.env.MAIL_USER;
const mailPass = process.env.MAIL_PASS;
const mailTo = process.env.MAIL_TO;

let transporter = null;
if (mailUser && mailPass) {
  transporter = nodemailer.createTransport({
    service: process.env.MAIL_SERVICE || 'gmail',
    auth: { user: mailUser, pass: mailPass }
  });
}

async function createAlert(type, message, payload = {}, severity = 'warning') {
  const alert = new Alert({ type, message, payload, severity });
  await alert.save();
  // fire email if configured
  if (transporter && mailTo) {
    try {
      await transporter.sendMail({
        from: mailUser,
        to: mailTo,
        subject: `[PAVITRAX] ${type.toUpperCase()}`,
        text: `${message}\n\nPayload: ${JSON.stringify(payload, null, 2)}`
      });
    } catch (err) {
      console.error('Email send failed', err.message);
    }
  }
  return alert;
}

async function list(req, res) {
  const alerts = await Alert.find().sort({ createdAt: -1 }).limit(100);
  res.json({ ok: true, data: alerts });
}

async function acknowledge(req, res) {
  const { id } = req.params;
  await Alert.findByIdAndUpdate(id, { acknowledged: true });
  res.json({ ok: true });
}

module.exports = { createAlert, list, acknowledge };

