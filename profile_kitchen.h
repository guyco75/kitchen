#define MQTT_CLIENT_ID "Kitchen-ESP8266"
#define TOPIC_PREFIX "home/f1/kitchen"
#define SERIAL_SPEED (57600)

static void subscribe() {
  client.subscribe(TOPIC_PREFIX "/light/+/set");
}

static void handle_mqtt_message(char *topic, char *str, unsigned int length) {
  char serial_str[40];

  if (!strcmp(TOPIC_PREFIX "/light/0/set", topic)) {
    snprintf(serial_str, sizeof(serial_str), "$0,%s,#", str);
    Serial.print(serial_str);
    client.publish("dbg" TOPIC_PREFIX, serial_str);
  }
}

static void handle_serial_cmd() {
  char topic[40];
  char payload[16];
  int cmd, l, br;

  cmd = ser_parser.get_next_token_int();
  if (cmd == 0) {
    l = ser_parser.get_next_token_int();
    if (l < 0 || 15 < l) {client.publish("dbg", "err: serial - light");return;}

    br = ser_parser.get_next_token_int();
    if (br < 0 || 100 < br) {client.publish("dbg", "err: serial - brightness");return;}

    snprintf(topic, sizeof(topic), TOPIC_PREFIX "/light/%d/update", l);
    snprintf(payload, sizeof(payload), "%d", br);
    client.publish(topic, payload);
  }
}

