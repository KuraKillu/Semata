// CustomArduino.h

#ifndef CUSTOM_ARDUINO_H
#define CUSTOM_ARDUINO_H

// Include only specific Arduino functions as needed
#include <inttypes.h>   // for standard types like uint8_t, uint16_t, etc.

// Forward declare or redefine specific functions as needed
// For example, if you only need `pinMode`, `digitalWrite`, and `Serial`
extern void pinMode(uint8_t pin, uint8_t mode);
extern void digitalWrite(uint8_t pin, uint8_t val);

// Include the Serial object if needed
extern HardwareSerial Serial;

#endif // CUSTOM_ARDUINO_H
