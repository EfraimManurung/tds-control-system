#include "Actuator_LED.h"

/* LED */
Actuator_LED::Actuator_LED(int pin) : _pin(pin) {}

bool Actuator_LED::init() {
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, LOW);

  return true;
}

void Actuator_LED::set_state(bool on) { digitalWrite(_pin, on ? HIGH : LOW); }