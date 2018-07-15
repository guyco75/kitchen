#define MQTT_CLIENT_ID "Kitchen-ESP8266"
#define TOPIC_PREFIX "home/f1/kitchen"
#define SERIAL_SPEED (57600)

static void subscribe() {
  client.subscribe(TOPIC_PREFIX "/#");
  client.subscribe(TOPIC_BCAST);
}

static void handle_mqtt_message(char *topic, char *str) {
  if (!strcmp(TOPIC_PREFIX "/light/set/0", topic)) {
    serial_out("$0,0,%s,#", str);
    client.publish("dbg" TOPIC_PREFIX, serial_out_buf);
  }
}

static void poll_device_state() {
  //place holder
}

static void handle_serial_cmd() {
  char topic[40];
  char payload[16];
  int32_t cmd, l, br;

  if (!ser_parser.get_next_token_int(&cmd)) {client.publish("dbg" TOPIC_PREFIX, "err: serial - cmd");return;}
  if (cmd == 0) {
    if (!ser_parser.get_next_token_int(&l) || l < 0 || 15 < l) {client.publish("dbg", "err: serial - light");return;}

    if (!ser_parser.get_next_token_int(&br) || br < 0 || 100 < br) {client.publish("dbg", "err: serial - brightness");return;}

    snprintf(topic, sizeof(topic), TOPIC_PREFIX "/light/update/%d", l);
    snprintf(payload, sizeof(payload), "%d", br);
    client.publish(topic, payload);
  }
}


