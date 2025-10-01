// Sensor_TDS.cpp
#include "Sensor_TDS.h"

Sensor_TDS::Sensor_TDS(uint8_t tds_sensor_pin)
    : _tds_sensor_pin(tds_sensor_pin) {}

bool Sensor_TDS::init() {
  pinMode(_tds_sensor_pin, INPUT);
  return true;
}

float Sensor_TDS::get_tds_value(float temperature) {
  static unsigned long analog_sample_timepoint = millis();
  if (millis() - analog_sample_timepoint > 40U) {
    analog_sample_timepoint = millis();
    _analog_buffer[_analog_buffer_index] = analogRead(_tds_sensor_pin);
    _analog_buffer_index = (_analog_buffer_index + 1) % SCOUNT;
  }

  static unsigned long compute_timepoint = millis();
  if (millis() - compute_timepoint > 800U) {
    compute_timepoint = millis();

    int sorted[SCOUNT];
    memcpy(sorted, _analog_buffer, sizeof(sorted));

    _average_voltage =
        get_median_num(sorted, SCOUNT) * (float)VREF / (float)_bit_value;

    float compensation_coeff = 1.0 + 0.02 * (temperature - 25.0);
    float compensation_voltage = _average_voltage / compensation_coeff;

    _tds_value = (133.42 * pow(compensation_voltage, 3) -
                  255.86 * pow(compensation_voltage, 2) +
                  857.39 * compensation_voltage) *
                 0.5;
  }

  return _tds_value;
}

inline int Sensor_TDS::get_median_num(const int *arr, size_t len) {
  int buf[len];
  memcpy(buf, arr, len * sizeof(int));

  for (size_t j = 0; j < len - 1; ++j) {
    for (size_t i = 0; i < len - j - 1; ++i) {
      if (buf[i] > buf[i + 1]) {
        int tmp = buf[i];
        buf[i] = buf[i + 1];
        buf[i + 1] = tmp;
      }
    }
  }
  if (len & 1)
    return buf[len / 2];
  return (buf[len / 2] + buf[len / 2 - 1]) / 2;
}