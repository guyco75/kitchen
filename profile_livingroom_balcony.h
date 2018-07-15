#define MQTT_CLIENT_ID "livingroom_balcony-ESP8266"
#define TOPIC_PREFIX "home/f1/livingroom/balcony"
#define SERIAL_SPEED (57600)

char str_buff[120];

static void subscribe() {
  client.subscribe(TOPIC_PREFIX "/#");
  client.subscribe(TOPIC_BCAST);
}

static void handle_mqtt_message(char *topic, char *str) {
  uint16_t id, percentage;
  int ret;

  ret = sscanf(topic, TOPIC_PREFIX "/rs/%d/set", &id);
  if (ret == 1) {
    snprintf(str_buff, sizeof(str_buff), "$rs,%d,%s#", id, str);
    Serial.print(str_buff);
    client.publish("dbg" TOPIC_PREFIX, str_buff);
  }
}

static void poll_device_state() {
  static char s[] = "$status_req#";
  Serial.print(s);
  client.publish("dbg" TOPIC_PREFIX, s);
}

static void handle_serial_cmd() {
  int cmd, l, br;

  if (strlen(ser_parser.rx_cmd_str) < sizeof(str_buff)) {
    snprintf(str_buff, sizeof(str_buff), "%s", ser_parser.rx_cmd_str);
    client.publish(TOPIC_PREFIX "/status_update", str_buff);
  } else {
    client.publish("dbg" TOPIC_PREFIX, "error: serial update len");
  }
}

