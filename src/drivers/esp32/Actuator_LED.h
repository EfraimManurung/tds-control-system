#pragma once

#include <Arduino.h>
#include <interface/ActuatorInterface.h>

class Actuator_LED : public ActuatorInterface {
private:
  int _pin;

public:
  Actuator_LED(int pin);
  bool init();
  void set_state(bool on);
};