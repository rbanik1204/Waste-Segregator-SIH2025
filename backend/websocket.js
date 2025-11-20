// websocket.js — socket.io bridge: broadcast mqtt telemetry to connected browser clients
const { Server } = require('socket.io');
const mqttBridge = require('./mqtt');
const config = require('./config/mqttConfig');

let io = null;

function init(server, mqttBridgeInstance) {
  io = new Server(server, {
    cors: { origin: '*' }
  });

  // start mqtt if not started
  mqttBridge.start();

  // on new telemetry, forward to clients
  mqttBridge.on('telemetry', (payload) => {
    // enrich payload if needed and emit
    io.emit('telemetry', payload);
  });

  io.on('connection', (socket) => {
    console.log('Socket connected', socket.id);

    socket.on('command', (cmd) => {
      // forward commands to mqtt topic
      const topic = (mqttBridge.topics && mqttBridge.topics.commands) || config.topics.commands || 'pavitrax/boat1/commands';
      mqttBridge.publish(topic, cmd);
      console.log('Forwarded command via MQTT', cmd);
    });

    socket.on('disconnect', () => {
      console.log('Socket disconnected', socket.id);
    });
  });

  return io;
}

module.exports = { init };
