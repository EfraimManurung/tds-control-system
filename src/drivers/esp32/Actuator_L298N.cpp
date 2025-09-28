#include "Actuator_L298N.h"

Actuator_L298N::Actuator_L298N(GPIOInterface *gpio_type, uint8_t ena,
                               uint8_t in1, uint8_t in2)
    : _gpio_type(gpio_type), _ena(ena), _in1(in1), _in2(in2) {}

bool Actuator_L298N::init() {
  /* this pin is always from ESP32 GPIOs */
  pinMode(_ena, OUTPUT);
  /* Meanwhile this pin is from the PCF8575 */
  _gpio_type->set(_in1, OUTPUT);
  _gpio_type->set(_in2, OUTPUT);

  if (!_gpio_type->init()) {
    Serial.println("Failed to initialize GPIO interface!");
    return false;
  }

  /* Initialize as forward direction in the setup first */
  this->set_forward_direction();

  return true;
}

void Actuator_L298N::set_speed(uint8_t speed) { analogWrite(_ena, speed); }

void Actuator_L298N::set_forward_direction() {
  _gpio_type->digital_write(_in1, HIGH);
  _gpio_type->digital_write(_in2, LOW);
}

void Actuator_L298N::set_reverse_direction() {
  _gpio_type->digital_write(_in1, LOW);
  _gpio_type->digital_write(_in2, HIGH);
}