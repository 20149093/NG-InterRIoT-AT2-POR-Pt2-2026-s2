
/**
 * Assessment: InterRIoT AT2 - Part 2
 * Student: Nelson Gomez
 * Semester: 2026/S2
 * Board: ESP32 Dev Module
 *
 * Test Wi-Fi and MQTT connectivity using Adafruit IO.
 */

#define DEBUG 1

#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

#include "secrets.h"
#include "debug.h"

// LED status indicators
#define LED_RED 4
#define LED_GREEN 13
#define LED_BLUE 18

// Wi-Fi client
WiFiClient client;

// MQTT client
Adafruit_MQTT_Client mqtt(
  &client,
  MQTT_BROKER,
  MQTT_PORT,
  AIO_USERNAME,
  AIO_KEY
);

// MQTT feed
Adafruit_MQTT_Publish deviceFeed(
  &mqtt,
  MQTT_FEED
);

void setup() {
pinMode(LED_RED, OUTPUT);
pinMode(LED_GREEN, OUTPUT);
pinMode(LED_BLUE, OUTPUT);

digitalWrite(LED_RED, HIGH);
digitalWrite(LED_GREEN, LOW);
digitalWrite(LED_BLUE, LOW);

  DEBUG_BEGIN(115200);
  delay(1000);

  DEBUG_PRINTLN("ESP32 started.");

  // Connect to Wi-Fi
  DEBUG_PRINTLN("Connecting to Wi-Fi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long startTime = millis();

  while (WiFi.status() != WL_CONNECTED &&
         millis() - startTime < 20000) {
    delay(500);
    DEBUG_PRINT(".");
  }

  if (WiFi.status() != WL_CONNECTED) {
    DEBUG_PRINTLN("Wi-Fi connection failed.");
    return;
  }

  DEBUG_PRINTLN("");
  DEBUG_PRINTLN("Wi-Fi connected!");

  //Turn off the red LED after wifi connection
  digitalWrite(LED_RED, LOW);

  DEBUG_PRINT("IP address: ");
  DEBUG_PRINTLN(WiFi.localIP());

  // Connect to MQTT
  DEBUG_PRINTLN("Connecting to MQTT...");

  int8_t result = mqtt.connect();

  if (result != 0) {
    DEBUG_PRINT("MQTT connection failed: ");
    DEBUG_PRINTLN(result);
    mqtt.disconnect();
    return;
  }

  DEBUG_PRINTLN("MQTT connected!");

  //Flash green LED three times when MQTT conntects
  for (int i = 0; i < 3; i++) {
    digitalWrite(LED_GREEN, HIGH);
    delay(100);
    digitalWrite(LED_GREEN, LOW);
    delay(100);
  }

  // Publish a test message
  if (deviceFeed.publish("ESP32 connected successfully")) {

    DEBUG_PRINTLN("Message published successfully!");

      //Flash blue LED once after successful MQTT publication
    digitalWrite(LED_BLUE, HIGH);
    delay(200);
    digitalWrite(LED_BLUE, LOW);

  } else {
    DEBUG_PRINTLN("Failed to publish message.");
  }
}

void loop() {
  // Connectivity test only.
}



