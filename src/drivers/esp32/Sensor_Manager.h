#pragma once

#include <Arduino.h>
#include <array>
#include <interface/SensorInterface.h>

constexpr size_t MAX_SENSOR_DEVICES = 8;

class Sensor_Manager : public SensorInterface {
private:
  std::array<SensorInterface *, MAX_SENSOR_DEVICES> _devices{};
  size_t _count = 0;

public:
  void add(SensorInterface *device);
  bool init();
};