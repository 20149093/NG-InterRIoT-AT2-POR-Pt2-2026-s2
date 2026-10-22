
#ifndef DEBUG_H
#define DEBUG_H

/**
 * Debug macros to provide serial debugging of device.
 */

#if DEBUG
  #define DEBUG_BEGIN(baud) Serial.begin(baud)
  #define DEBUG_PRINT(value) Serial.print(value)
  #define DEBUG_PRINTLN(value) Serial.println(value)
#else
  #define DEBUG_BEGIN(baud)
  #define DEBUG_PRINT(value)
  #define DEBUG_PRINTLN(value)
#endif

#endif // DEBUG_H
