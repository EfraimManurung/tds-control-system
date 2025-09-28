#pragma once

#include <Arduino.h>
#include <interface/ActuatorInterface.h>

class Actuator_Relay : public ActuatorInterface {
private:
  int _pin;

public:
  Actuator_Relay(int pin);
  bool init();
  void set_state(bool on);
};