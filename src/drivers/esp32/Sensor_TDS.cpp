#include "Sensor_TDS.h"

Sensor_TDS::Sensor_TDS(uint8_t tds_sensor_pin)
    : _tds_sensor_pin(tds_sensor_pin) {}

bool Sensor_TDS::init() {
  _gravity_tds.setPin(_tds_sensor_pin);
  _gravity_tds.setAref(_reference_voltage);
  _gravity_tds.setAdcRange(_bit_value);
  _gravity_tds.begin(); /* initialization */

  return true;
}

float Sensor_TDS::get_tds_value(float temperature) {
  // temperature = readTemperature();  //add your temperature sensor and read it
  _gravity_tds.setTemperature(
      temperature); // set the temperature and execute temperature compensation
  _gravity_tds.update(); // sample and calculate

  return _tds_value = _gravity_tds.getTdsValue(); // then get the value
}