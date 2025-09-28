#include "Local_WiFi.h"

Local_WiFi::Local_WiFi() {}
Local_WiFi::~Local_WiFi() {}

bool Local_WiFi::init(const char *ssid, const char *password) {
  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting ESP32 AP (Access Point). . .");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  /* get the ip_address */
  _ip_address = WiFi.softAPIP();

  Serial.println("\nConnected: " + _ip_address.toString());
  return true;
}

std::string Local_WiFi::get_ip_address() {
  return std::string(_ip_address.toString().c_str());
}