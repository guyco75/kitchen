#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "serial_parser.h"
#include "credentials.h"

#define BUILTIN_LED (2)
#define MQTT_CLIENT_ID "Kitchen-ESP8266"
#define TOPIC_PREFIX "home/f1/kitchen"

WiFiClient espClient;
PubSubClient client(mqtt_server, 1883, espClient);

void setup_wifi() {
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //TODO: blink
    Serial.print(".");
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  char serial_str[40];
  char str[16];

  if (length >= sizeof(str)) {
    client.publish("dbg", "err: callback - length");
    return;
  }
  memcpy(str, payload, length);
  str[length] = '\0';

  if (!strcmp(TOPIC_PREFIX "/light/0/set", topic)) {
    snprintf(serial_str, sizeof(serial_str), "$0,%s,#", str);
    Serial.print(serial_str);
    client.publish("dbg" TOPIC_PREFIX, serial_str);
  }
}

void reconnect() {
  while (!client.connected()) {
    if (client.connect(MQTT_CLIENT_ID)) {
      client.subscribe(TOPIC_PREFIX "/light/+/set");
      client.publish(TOPIC_PREFIX "/status", "alive");
    } else {
      //TODO: blink
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  Serial.begin(9600);
  setup_wifi(); // TODO: Anything to do in reconnect?
  client.setCallback(callback);
}

SerialParser serParser(128);

void handleSerialCmd() {
  char topic[40];
  char payload[16];
  int cmd, l, br;

  cmd = serParser.getNextToken().toInt();
  if (cmd == 0) {
    l = serParser.getNextToken().toInt();
    if (l < 0 || 15 < l) {client.publish("dbg", "err: serial - light");return;}
  
    br = serParser.getNextToken().toInt();
    if (br < 0 || 100 < br) {client.publish("dbg", "err: serial - brightness");return;}

    snprintf(topic, sizeof(topic), TOPIC_PREFIX "/light/%d/update", l);
    snprintf(payload, sizeof(payload), "%d", br);
    client.publish(topic, payload);
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (serParser.processSerial()) {
    handleSerialCmd();
  }
}

