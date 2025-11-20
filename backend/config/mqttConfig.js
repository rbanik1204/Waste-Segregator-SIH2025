module.exports = {
    url: process.env.MQTT_URL || 'mqtt://localhost:1883',
    options: {
      clientId: process.env.MQTT_CLIENT_ID || 'pavitrax_backend_' + Math.random().toString(16).substr(2,6)
      // username, password if your broker requires it
    },
    topics: {
      telemetry: process.env.MQTT_TELEMETRY_TOPIC || 'pavitrax/boat1/telemetry',
      commands: process.env.MQTT_COMMANDS_TOPIC || 'pavitrax/boat1/commands'
    }
};
  