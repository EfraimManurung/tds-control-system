// Display_SSD1306.h
#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>
#include <Wire.h>
#include <interface/DisplayInterface.h>
#include <string>

#define SSD1306_ADDRESS 0x3C

class Display_SSD1306 : public DisplayInterface {
public:
  bool init();
  bool set_ip_address(const std::string &ip);
  bool set_elapsed_time(const std::string &elapsed);
  bool set_current_tds_value(uint16_t tds_value);
  bool set_setted_tds_value(uint16_t setted_tds_value);

private:
  Adafruit_SSD1306 _oled{128, 64, &Wire, -1};

  std::string _ip_address;
  std::string _elapsed_time;

  // TDS values (uint8_t kept to match your API; change to larger type if
  // needed)
  uint16_t _current_tds_value = 0;
  uint16_t _setted_tds_value = 0;

  void refresh_display();
};