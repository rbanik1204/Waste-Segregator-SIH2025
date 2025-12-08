/*
 * WiFi Configuration
 * Edit these values for your network
 */

#ifndef WIFI_CONFIG_H
#define WIFI_CONFIG_H

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";
const char* serverUrl = "http://192.168.1.100:4000/api/sensors/upload";
const char* mqttBroker = "192.168.1.100";
const int mqttPort = 1883;

#endif

