#define MQTT_CLIENT_ID "livingroom_balcony-ESP8266"
#define TOPIC_PREFIX "home/f1/livingroom/balcony"
#define SERIAL_SPEED (57600)

static void subscribe() {
  client.subscribe(TOPIC_PREFIX "/#");
  client.subscribe(TOPIC_BCAST);
}

static void handle_mqtt_message(char *topic, char *str) {
  uint16_t id, percentage;
  int ret;

  ret = sscanf(topic, TOPIC_PREFIX "/rs/%d/set", &id);
  if (ret == 1) {
    serial_out("$rs,%d,%s#", id, str);
    client.publish("dbg" TOPIC_PREFIX, serial_out_buf);
  }
}

static void poll_device_state() {
  static char s[] = "$status_req#";
  Serial.print(s);
  client.publish("dbg" TOPIC_PREFIX, s);
}

static void handle_serial_cmd() {
  static char payload[40];
  int cmd, l, br;

  if (strlen(ser_parser.rx_cmd_str) < sizeof(payload)) {
    snprintf(payload, sizeof(payload), "%s", ser_parser.rx_cmd_str);
    client.publish(TOPIC_PREFIX "/status_update", payload);
  } else {
    client.publish("dbg" TOPIC_PREFIX, "error: serial update len");
  }
}

