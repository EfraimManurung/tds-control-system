#include "GPIO_ESP32.h"

bool GPIO_ESP32::init() { return true; }

void GPIO_ESP32::set(uint8_t pin, uint8_t mode) { pinMode(pin, mode); }

void GPIO_ESP32::digital_write(uint8_t pin, bool value) {
  digitalWrite(pin, value);
}