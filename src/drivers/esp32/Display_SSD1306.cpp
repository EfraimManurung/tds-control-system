// Display_SSD1306.cpp
#include "Display_SSD1306.h"

bool Display_SSD1306::init() {
  if (!_oled.begin(SSD1306_SWITCHCAPVCC, SSD1306_ADDRESS)) {
    return false;
  }
  _oled.clearDisplay();
  _oled.setTextColor(SSD1306_WHITE);
  refresh_display();
  return true;
}

bool Display_SSD1306::set_ip_address(const std::string &ip) {
  _ip_address = ip;
  refresh_display();
  return true;
}

bool Display_SSD1306::set_elapsed_time(const std::string &elapsed) {
  _elapsed_time = elapsed;
  refresh_display();
  return true;
}

bool Display_SSD1306::set_current_tds_value(uint16_t tds_value) {
  _current_tds_value = tds_value;
  refresh_display();
  return true;
}

bool Display_SSD1306::set_setted_tds_value(uint16_t setted_tds_value) {
  _setted_tds_value = setted_tds_value;
  refresh_display();
  return true;
}

void Display_SSD1306::refresh_display() {
  _oled.clearDisplay();
  _oled.setTextSize(1);
  _oled.setTextColor(SSD1306_WHITE);

  // Row 0 - Current TDS
  _oled.setCursor(0, 0);
  _oled.print("Current TDS: ");
  _oled.print(_current_tds_value);

  // Row 2 - Set TDS
  _oled.setCursor(0, 16);
  _oled.print("Set TDS: ");
  _oled.print(_setted_tds_value);

  // Row 3 - Elapsed time (if any)
  if (!_elapsed_time.empty()) {
    _oled.setCursor(0, 32);
    _oled.print("Elapsed: ");
    _oled.print(_elapsed_time.c_str());
  }

  // Row 6 - IP address (if any)
  if (!_ip_address.empty()) {
    _oled.setCursor(0, 48);
    _oled.print("IP: ");
    _oled.print(_ip_address.c_str());
  }

  _oled.display();
}