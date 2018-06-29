#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define SERIAL_CMD_BUF_SIZE (120)
#include "serial_parser/serial_parser.h"
serial_parser ser_parser;

#include "credentials.h"

#define BUILTIN_LED (2)

WiFiClient espClient;
PubSubClient client(mqtt_server, 1883, espClient);

#include "kitchen_profile.h"

void setup_wifi() {
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //TODO: blink
    Serial.print(".");
  }
}

void callback(char *topic, byte *payload, unsigned int length) {
  char str[80];

  if (length >= sizeof(str)) {
    client.publish(TOPIC_PREFIX, "err: callback - length");
    return;
  }
  memcpy(str, payload, length);
  str[length] = '\0';

  handle_mqtt_message(topic, str, length);
}

void reconnect() {
  char alive[40];
  while (!client.connected()) {
    if (client.connect(MQTT_CLIENT_ID)) {
      subscribe();
      snprintf(alive, sizeof(alive), "alive %d.%d.%d.%d", WiFi.localIP()[0],WiFi.localIP()[1],WiFi.localIP()[2],WiFi.localIP()[3]);
      client.publish(TOPIC_PREFIX "/status", alive);
    } else {
      //TODO: blink
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup() {
  pinMode(BUILTIN_LED, OUTPUT);
  Serial.begin(SERIAL_SPEED);
  setup_wifi(); // TODO: Anything to do in reconnect?
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  if (ser_parser.process_serial()) {
    handle_serial_cmd();
  }
}

