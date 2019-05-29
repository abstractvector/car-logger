#include "recorder.h"

#include <MQTT.h>
#include <WiFi.h>

#include "config.h"

WiFiClient wifi;
MQTTClient mqtt;

unsigned long _lastPublishTime = 0;

Recorder::Recorder() { }

void Recorder::begin()
{

  // WiFi
  Serial.println("");
  Serial.print(F("Configuring WiFi for SSID: "));
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("[Recorder] WiFi connected");
  Serial.print("[Recorder] IP address: ");
  Serial.println(WiFi.localIP());

  // MQTT
  Serial.print(F("[Recorder] Configuring MQTT broker: "));
  Serial.print(MQTT_SERVER);
  Serial.print(F(":"));
  Serial.println(MQTT_PORT);
  mqtt.begin(MQTT_SERVER, MQTT_PORT, wifi);

  Serial.print(F("[Recorder] Using MQTT client ID: "));
  Serial.println(THING_ID);

  Serial.print(F("[Recorder] Connecting..."));
  while (!mqtt.connect(THING_ID)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println(F("."));

  Serial.println(F("[Recorder] Connected to MQTT"));
}

bool Recorder::send(char* message)
{
  mqtt.publish("things/smartrv/obd-logger/measurement", message);
  return true;
}

void Recorder::loop()
{
  heartbeat();
}

void Recorder::heartbeat()
{
  unsigned long currentMillis = millis();

  if (currentMillis - _lastPublishTime >= 10000) {
    // do our publishing here
    _lastPublishTime = currentMillis;

    Serial.println(F("[Recorder] Sending heartbeat"));

    mqtt.publish("things/smartrv/obd-logger/id", THING_ID);
    mqtt.publish("things/smartrv/obd-logger/alive", "1");
    mqtt.publish("things/smartrv/obd-logger/uptime", String(currentMillis));
  }
}
