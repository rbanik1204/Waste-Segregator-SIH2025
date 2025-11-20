// server.js — start express, socket.io and wire mqtt bridge
const express = require('express');
const http = require('http');
const cors = require('cors');
const bodyParser = require('body-parser');

const mqttBridge = require('./mqtt'); // starts mqtt connection
const websocket = require('./websocket'); // sets up socket.io (needs server)
const telemetryRoutes = require('./routes/telemetryRoutes');
const controlRoutes = require('./routes/controlRoutes');
const reportRoutes = require('./routes/reportRoutes');

const app = express();
app.use(cors());
app.use(bodyParser.json());

app.use('/api/telemetry', telemetryRoutes);
app.use('/api/control', controlRoutes);
app.use('/api/reports', reportRoutes);

// static serve (optional) so frontend can be hosted from backend during dev
app.use('/static', express.static(__dirname + '/public'));

const server = http.createServer(app);
const io = websocket.init(server, mqttBridge); // pass mqttBridge for publishing commands if needed

const PORT = process.env.PORT || 4000;
server.listen(PORT, () => {
  console.log(`Backend server listening on http://localhost:${PORT}`);
});
