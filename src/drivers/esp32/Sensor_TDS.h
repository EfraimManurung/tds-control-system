#include "Arduino.h"
#include "GravityTDS.h"
#include <EEPROM.h>
#include <interface/SensorInterface.h>

class Sensor_TDS : public SensorInterface {
public:
  Sensor_TDS(uint8_t tds_sensor_pin);
  bool init();
  /* default is in room temperature which is 25.0 */
  float get_tds_value(float temperature = 25.0);

private:
  GravityTDS _gravity_tds;

  uint8_t _tds_sensor_pin;
  float _tds_value = 0;
  float _reference_voltage =
      5.0;                    // reference voltage on ADC, default 5.0V on ESP32
  uint16_t _bit_value = 4096; // 1024 for 10bit ADC;4096 for 12bit ADC
};