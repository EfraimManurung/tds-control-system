#pragma once

#include <Arduino.h>
#include <IPAddress.h>
// Load Wi-Fi library
#include <WiFi.h>
#include <interface/NetworkInterface.h>

class Local_WiFi : public NetworkInterface {
public:
  Local_WiFi();
  ~Local_WiFi();

  bool init(const char *ssid, const char *password); /* overload */
  std::string get_ip_address();

private:
  IPAddress _ip_address;
};