// Sensor_TDS.h
#pragma once
#include "Arduino.h"
#include <interface/SensorInterface.h>

#define SCOUNT 30
#define VREF 5.0

class Sensor_TDS : public SensorInterface {
public:
  explicit Sensor_TDS(uint8_t tds_sensor_pin);
  bool init();
  float get_tds_value(float temperature = 25.0);

private:
  uint8_t _tds_sensor_pin;
  float _tds_value = 0;
  uint16_t _bit_value = 4096;

  int _analog_buffer[SCOUNT];
  int _analog_buffer_index = 0;

  float _average_voltage = 0;

  static inline int get_median_num(const int *arr, size_t len);
};
