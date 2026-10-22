/**
 * Assessment: InterRIoT AT2 - Part 2
 * Student: Nelson Gomez
 * Semester: 2026/S2
 * Board: ESP32 Dev Module
 */

#define DEBUG 1

#include "debug.h"

void setup() {
  DEBUG_BEGIN(115200);
  delay(1000);

  DEBUG_PRINTLN("ESP32 debugging enabled.");
}

void loop() {
  DEBUG_PRINTLN("ESP32 is running.");
  delay(2000);
}


