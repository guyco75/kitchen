#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#define SERIAL_CMD_BUF_SIZE (120)
#define SERIAL_OUT_BUF_SIZE (80)
#include "serial_parser/serial_parser.h"

#include "credentials.h"  // ssid, password, mqtt_server

#define TOPIC_BCAST "home/bcast"

WiFiClient espClient;
PubSubClient client(mqtt_server, 1883, espClient);

#include "profile_kitchen.h"
//#include "profile_livingroom_balcony.h"

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

  if (!strcmp(TOPIC_BCAST, topic)) {
    if (!strcmp("status_req", str))
      poll_device_state();
  } else {
    handle_mqtt_message(topic, str);
  }
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
  Serial.begin(SERIAL_SPEED);
  setup_wifi(); // TODO: Anything to do in reconnect?
  client.setCallback(callback);
}

void loop() {
  if (!client.connected()) {
    reconnect();
    poll_device_state();
  }
  client.loop();

  if (ser_parser.process_serial()) {
    handle_serial_cmd();
  }
}

