#pragma once

class GPIOInterface {
public:
  virtual bool init() = 0;
  virtual void set(uint8_t pin, uint8_t mode) = 0;
  virtual void digital_write(uint8_t pin, bool value) = 0;
  virtual ~GPIOInterface() {};
};