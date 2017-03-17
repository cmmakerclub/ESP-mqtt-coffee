#include <Arduino.h>
#include "MqttConnector.h"

extern String MQTT_CLIENT_ID;

#define relayPin 15
int pin_state;
extern MqttConnector* mqtt;
extern char myName[40];

#include <Servo.h>
Servo myservo;
#define SERVO 14


void trig_servo() {
  myservo.write(10);
  delay(1000);
  myservo.write(50);
}

void register_receive_hooks() {
  mqtt->on_subscribe([&](MQTT::Subscribe * sub) -> void {
    Serial.printf("myName = %s \r\n", myName);
    sub->add_topic(MQTT_PREFIX + "/" + myName + "/$/+");
    sub->add_topic(MQTT_PREFIX + "/" + MQTT_CLIENT_ID + "/$/+");
  });

  mqtt->on_before_message_arrived_once([&](void) {
    pinMode(15, OUTPUT);
    myservo.attach(SERVO);
    myservo.write(50);
  });

  mqtt->on_message([&](const MQTT::Publish & pub) { });

  mqtt->on_after_message_arrived([&](String topic, String cmd, String payload) {
    Serial.printf("topic: %s\r\n", topic.c_str());
    Serial.printf("cmd: %s\r\n", cmd.c_str());
    Serial.printf("payload: %s\r\n", payload.c_str());
    if (cmd == "$/command") {
      if (payload == "ON") {
        digitalWrite(relayPin, HIGH);
        digitalWrite(LED_BUILTIN, LOW);
        pin_state = 1;
        trig_servo();
      }
      else if (payload == "OFF") {
        digitalWrite(relayPin, LOW);
        digitalWrite(LED_BUILTIN, HIGH);
        pin_state = 0;
        trig_servo();
      }
    }
    else if (cmd == "$/reboot") {
      ESP.reset();
    }
    else {

    }
  });
}
