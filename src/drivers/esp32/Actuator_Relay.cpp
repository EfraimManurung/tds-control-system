#include "Actuator_Relay.h"

Actuator_Relay::Actuator_Relay(int pin) : _pin(pin) {}

bool Actuator_Relay::init() {
  pinMode(_pin, OUTPUT);
  digitalWrite(_pin, LOW);

  return true;
}

void Actuator_Relay::set_state(bool on) { digitalWrite(_pin, on ? LOW : HIGH); }