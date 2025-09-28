#pragma once

#include "PCF8575.h"
#include <Arduino.h>
#include <interface/ActuatorInterface.h>
#include <interface/GPIOInterface.h>

class Actuator_L298N : public ActuatorInterface {
public:
  Actuator_L298N(GPIOInterface *gpio_type, uint8_t ena, uint8_t in1,
                 uint8_t in2);
  bool init();
  void set_speed(uint8_t speed); // PWM control
  void set_forward_direction();  // go forward
  void set_reverse_direction();  // go reverse

private:
  uint8_t _ena, _in1, _in2;
  GPIOInterface *_gpio_type;
};