
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

// HC-SR04 ultrasonic sensor
#define TRIG_PIN 5
#define ECHO_PIN 19
#define ECHO_TIMEOUT_MICROSECONDS 30000UL

// Deep Sleep configuration
#define SLEEP_TIME_SECONDS 1800
#define MICROSECONDS_PER_SECOND 1000000ULL

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

// Distance measurement feed
Adafruit_MQTT_Publish distanceFeed(
  &mqtt,
  AIO_USERNAME "/feeds/inter-riot-at2-data"
);

void setup() {
pinMode(LED_RED, OUTPUT);
pinMode(LED_GREEN, OUTPUT);
pinMode(LED_BLUE, OUTPUT);
pinMode(TRIG_PIN, OUTPUT);
pinMode(ECHO_PIN, INPUT);

digitalWrite(TRIG_PIN, LOW);

digitalWrite(LED_RED, HIGH);
digitalWrite(LED_GREEN, LOW);
digitalWrite(LED_BLUE, LOW);

  DEBUG_BEGIN(115200);
  delay(1000);

  // Identify whether the ESP32 woke up from Deep Sleep
  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_TIMER) {
    DEBUG_PRINTLN("ESP32 woke up from Deep Sleep timer.");
  } else {
    DEBUG_PRINTLN("ESP32 started normally.");
  }

  DEBUG_PRINTLN("ESP32 started.");

  // Connect to Wi-Fi
  DEBUG_PRINTLN("Connecting to Wi-Fi...");
  WiFi.mode(WIFI_STA);

// Wi-Fi retry configuration
const int MAX_WIFI_ATTEMPTS = 5;
const int WIFI_RETRY_DELAYS[] = {0, 5, 8, 11, 14};

for (int attempt = 0; attempt < MAX_WIFI_ATTEMPTS; attempt++) {

  if (attempt > 0) {
    DEBUG_PRINT("Waiting before retry: ");
    DEBUG_PRINTLN(WIFI_RETRY_DELAYS[attempt]);
    delay(WIFI_RETRY_DELAYS[attempt] * 1000);
  }

  DEBUG_PRINT("Wi-Fi attempt: ");
  DEBUG_PRINTLN(attempt + 1);

  WiFi.disconnect();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long startTime = millis();

  while (WiFi.status() != WL_CONNECTED &&
         millis() - startTime < 10000) {
    delay(500);
    DEBUG_PRINT(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    break;
  }

  DEBUG_PRINTLN("");
  DEBUG_PRINTLN("Wi-Fi attempt failed.");
}

  if (WiFi.status() != WL_CONNECTED) {
    DEBUG_PRINTLN("Wi-Fi connection failed.");
      // Disconnect Wi-Fi before entering Deep Sleep
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);

  // Enter Deep Sleep and retry after waking up
    enterDeepSleep();
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

// MQTT retry configuration
const int MAX_MQTT_ATTEMPTS = 5;
const int MQTT_RETRY_DELAYS[] = {0, 7, 11, 15, 19};

bool mqttConnected = false;

for (int attempt = 0; attempt < MAX_MQTT_ATTEMPTS; attempt++) {

  if (attempt > 0) {
    DEBUG_PRINT("Waiting before MQTT retry: ");
    DEBUG_PRINTLN(MQTT_RETRY_DELAYS[attempt]);
    delay(MQTT_RETRY_DELAYS[attempt] * 1000);
  }

  DEBUG_PRINT("MQTT attempt: ");
  DEBUG_PRINTLN(attempt + 1);

  int8_t result = mqtt.connect();

  if (result == 0) {
    mqttConnected = true;
    break;
  }

  DEBUG_PRINT("MQTT connection failed: ");
  DEBUG_PRINTLN(result);

  mqtt.disconnect();
}

// Enter Deep Sleep if all MQTT attempts fail
if (!mqttConnected) {
  DEBUG_PRINTLN("All MQTT connection attempts failed.");

  mqtt.disconnect();
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);

  enterDeepSleep();
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

  // Measure and publish the distance before entering Deep Sleep
  float distanceCm = measureDistanceCm();

  if (distanceCm >= 0) {
    DEBUG_PRINT("Distance: ");
    DEBUG_PRINT(distanceCm);
    DEBUG_PRINTLN(" cm");

    if (distanceFeed.publish(distanceCm)) {
      DEBUG_PRINTLN("Distance published successfully!");
    } else {
      DEBUG_PRINTLN("Failed to publish distance.");
    }
  } else {
    DEBUG_PRINTLN("Distance measurement timed out.");
  }

  // Wait five seconds before entering Deep Sleep
DEBUG_PRINTLN("Waiting 5 seconds before Deep Sleep...");
delay(5000);

// Disconnect MQTT and Wi-Fi
mqtt.disconnect();
WiFi.disconnect();
WiFi.mode(WIFI_OFF);

// Enter Deep Sleep
enterDeepSleep();
}

void loop() {
  // Connectivity test only.
}

float measureDistanceCm() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  unsigned long duration = pulseIn(
    ECHO_PIN,
    HIGH,
    ECHO_TIMEOUT_MICROSECONDS
  );

  if (duration == 0) {
    return -1.0;
  }

  return duration * 0.0343 / 2.0;
}

void enterDeepSleep() {
  DEBUG_PRINTLN("Entering Deep Sleep...");

  Serial.flush();

  esp_sleep_enable_timer_wakeup(
    SLEEP_TIME_SECONDS * MICROSECONDS_PER_SECOND
  );

  esp_deep_sleep_start();
}



