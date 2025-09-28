#include "Actuator_Manager.h"

/*
  Concept overview:
  - ActuatorManager is a manager class (composite pattern).
  - Holds multiple ActuatorInterface* pointers (Relay, Motor, LED, etc.).
  - All devices inherit from ActuatorInterface and implement init().
*/

// Add device if capacity allows
void Actuator_Manager::add(ActuatorInterface *device) {
  if (_count < MAX_ACTUATOR_DEVICES) {
    _devices[_count++] = device;
  } else {
    Serial.println("[ActuatorManager] Device limit reached!");
  }
}

// Initialize all registered devices
bool Actuator_Manager::init() {
  bool success = true;

  for (size_t i = 0; i < _count; i++) {
    auto device = _devices[i];
    if (device == nullptr) {
      Serial.println("[ActuatorManager] Null device pointer!");
      success = false;
      continue;
    }

    if (!device->init()) {
      Serial.println("[ActuatorManager] A device failed to init()");
      success = false;
    }
  }

  return success;
}