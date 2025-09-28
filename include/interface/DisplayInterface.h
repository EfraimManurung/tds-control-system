#pragma once

#include <stdint.h>
#include <string>

class DisplayInterface {
public:
  virtual bool init() = 0;
  virtual bool set_ip_address(const std::string &ip) = 0;
  virtual bool set_elapsed_time(const std::string &elapsed) = 0;
  virtual bool set_current_tds_value(uint16_t tds_value) = 0;
  virtual bool set_setted_tds_value(uint16_t setted_tds_value) = 0;
};