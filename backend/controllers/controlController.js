const mqttBridge = require('../mqtt');
const config = require('../config/mqttConfig');
const BoatStatus = require('../models/BoatStatus');

function sendCommand(commandObj) {
  // validation or enrichment can go here
  const topic = (mqttBridge.topics && mqttBridge.topics.commands) || config.topics.commands || 'pavitrax/boat1/commands';
  mqttBridge.publish(topic, commandObj);
  BoatStatus.findOneAndUpdate(
    { boatId: commandObj.boat_id || 'boat-1' },
    {
      lastCommand: commandObj,
      mode: commandObj.mode ? commandObj.mode : undefined,
      conveyors: commandObj.conveyors ? commandObj.conveyors : undefined,
      updatedAt: new Date()
    },
    { upsert: true }
  ).catch((err) => console.error('Failed to store boat status', err.message));
  return { sent: true, topic, command: commandObj };
}

module.exports = { sendCommand };
