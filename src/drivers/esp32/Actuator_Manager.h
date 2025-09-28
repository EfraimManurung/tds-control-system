#pragma once

#include <Arduino.h>
#include <array>
#include <interface/ActuatorInterface.h>

constexpr size_t MAX_ACTUATOR_DEVICES = 8;

class Actuator_Manager : public ActuatorInterface {
private:
  std::array<ActuatorInterface *, MAX_ACTUATOR_DEVICES> _devices{};
  size_t _count = 0;

public:
  void add(ActuatorInterface *device);
  bool init();
};