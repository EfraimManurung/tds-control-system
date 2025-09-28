#include "Sensor_Manager.h"

// Add device if capacity allows
void Sensor_Manager::add(SensorInterface *device) {
  if (_count < MAX_SENSOR_DEVICES) {
    _devices[_count++] = device;
  } else {
    Serial.println("[SensorManager] Device limit reached!");
  }
}

// Initialize all registered devices
bool Sensor_Manager::init() {
  bool success = true;

  for (size_t i = 0; i < _count; i++) {
    auto device = _devices[i];
    if (device == nullptr) {
      Serial.println("[SensorManager] Null device pointer!");
      success = false;
      continue;
    }

    if (!device->init()) {
      Serial.println("[SensorManager] A device failed to init()");
      success = false;
    }
  }

  return success;
}