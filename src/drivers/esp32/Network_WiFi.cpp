#include "Network_WiFi.h"

Network_WiFi::Network_WiFi() {}
Network_WiFi::~Network_WiFi() {}

bool Network_WiFi::init(const char *ssid, const char *password) {
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");

  unsigned long start = millis();
  const unsigned long timeout = 15000; // 10 seconds max wait

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    if (millis() - start >= timeout) {
      Serial.println("\nWiFi connect timeout");
      return false;
    }
  }

  /* get the ip_address */
  _ip_address = WiFi.localIP();

  Serial.println("\nConnected: " + _ip_address.toString());
  return true;
}

std::string Network_WiFi::get_ip_address() {
  return std::string(_ip_address.toString().c_str());
}