const mqttBridge = require('../mqtt');
const config = require('../config/mqttConfig');

function sendCommand(commandObj) {
  // validation or enrichment can go here
  const topic = (mqttBridge.topics && mqttBridge.topics.commands) || config.topics.commands || 'pavitrax/boat1/commands';
  mqttBridge.publish(topic, commandObj);
  return { sent: true, topic, command: commandObj };
}

module.exports = { sendCommand };
