#pragma once
#include <Arduino.h>
#include <interface/GPIOInterface.h>

class GPIO_ESP32 : public GPIOInterface {
public:
  bool init();
  void set(uint8_t pin, uint8_t mode);
  void digital_write(uint8_t pin, bool value);
};