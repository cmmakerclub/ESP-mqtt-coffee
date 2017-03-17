#include <Arduino.h>
#include "CMMC_Blink.hpp"
#include "CMMC_Interval.hpp"
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
//#include <MQTT_OTA.hpp>
#include <DHT.h>
#include <MqttConnector.h>
#include "init_mqtt.h"
#include <Ticker.h>
#include <Servo.h> 
Servo myservo;

#define SERVO 14

boolean stateCoffee = false;
#include "_publish.h"
#include "_receive.h"
#include "RunningAverage.h"

const char* MQTT_HOST        = "mqtt.espert.io";
const char* MQTT_USERNAME    = "";
const char* MQTT_PASSWORD    = "";
const char* MQTT_CLIENT_ID   = "";
const char* MQTT_PREFIX      = "/CMMC";
const int MQTT_PORT           = 1883;
const int PUBLISH_EVERY       = 6000;

/* DEVICE DATA & FREQUENCY */
const char *DEVICE_NAME = "CMMC-ROOM-COFFEE";

// Example testing sketch for various DHT humidity/temperature sensors
// Written by ladyada, public domain

#define DHTPIN 12     // what digital pin we're connected to

#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);

CMMC_Blink blinker;
CMMC_Interval timer001;

MqttConnector *mqtt;

/* WIFI INFO */
#ifndef WIFI_SSID
  #define WIFI_SSID        "ESPERT-3020"
  #define WIFI_PASSWORD    "espertap"
#endif


Ticker ticker;
RunningAverage bucket(50);

float t_dht = 0;
float h_dht = 0;

static void read_dht() {
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);
  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
   Serial.println("Failed to read from DHT sensor!");
   return;
  }
  else {
    t_dht = t;
    h_dht = h;
    Serial.print("Temp: ");
    Serial.println(t_dht);
    Serial.print("Humid: ");
    Serial.println(h_dht);
  }
}

void init_hardware()
{
  Serial.begin(115200);
  delay(10);
  Serial.println();
  Serial.println("Starting...");
  myservo.attach(SERVO);
  myservo.write(50);
  pinMode(LED_BUILTIN, OUTPUT);
  blinker.init();
  blinker.blink(50, LED_BUILTIN);
  delay(200);
  bucket.clear(); // explicitly start cleans
  dht.begin();
}

void setup()
{
  init_hardware();

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while(WiFi.status() != WL_CONNECTED) {
    Serial.printf ("Connecting to %s:%s\r\n", WIFI_SSID, WIFI_PASSWORD);
    delay(300);
  }

  Serial.println("WiFi Connected.");
  delay(50);
  blinker.detach();
  digitalWrite(LED_BUILTIN, HIGH);

  init_mqtt();
}

void loop()
{
  mqtt->loop();
//  timer001.every_ms(2000, read_dht);
}
