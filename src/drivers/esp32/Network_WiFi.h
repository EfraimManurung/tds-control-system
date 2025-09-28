#pragma once

#include <Arduino.h>
#include <IPAddress.h>
#include <WiFi.h>
#include <interface/NetworkInterface.h>

class Network_WiFi : public NetworkInterface {
public:
  Network_WiFi();
  ~Network_WiFi();

  bool init(const char *ssid, const char *password); /* overload */
  std::string get_ip_address();

private:
  IPAddress _ip_address;
};