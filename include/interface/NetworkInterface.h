#pragma once

class NetworkInterface {

public:
  virtual bool init(const char *ssid, const char *password) = 0;
  virtual std::string get_ip_address() = 0;
};