/**
 *
 * Total Dissolved Solids (TDS) Control System
 *
 * Author: Efraim Manurung
 * Email: efraim.manurung@gmail.com
 *
 * Changelog:
 *
 * [28-09-2025]
 * -  Added basic, sensors and actuators classes and basic functions of the main
 * app.
 *
 * [01-09-2025]
 *  - First initialization.
 */

#include <Arduino.h>

#include "TDSControlSystem.h"
#include <drivers/esp32/Actuator_Manager.h>
#include <drivers/esp32/Display_SSD1306.h>

#ifdef USE_LOCAL_WIFI
#include <drivers/esp32/Local_WiFi.h>
#else
#include <drivers/esp32/Network_WiFi.h>
#endif

#include <drivers/esp32/Sensor_Manager.h>
#include <drivers/esp32/Website_Server.h>

// === WiFi Configuration ===
#define DEFAULT_SSID "PLASMART-Access-Point"
#define DEFAULT_PASS "plasmart1123"

// === Module Declarations ===
Actuator_Manager act_mgr; // Manages all actuators (LED, Motors, Relays)
Display_SSD1306 ssd1306;  // OLED display driver
Sensor_Manager
    sens_mgr; // Manages all sensors (keypads, buttons, water level, etc)

#ifdef USE_LOCAL_WIFI
Local_WiFi wifi;
#else
Network_WiFi wifi; // WiFi connectivity manager
#endif

Website_Server web; // Web interface for configuration and monitoring

// === Core System Instance ===
TDSControlSystem tds_cs(web, ssd1306, wifi, sens_mgr, act_mgr, DEFAULT_SSID,
                        DEFAULT_PASS);

void setup() {
  Serial.begin(115200);

  Serial.println("Hi, from main app!");

  tds_cs.init();
}

void loop() { tds_cs.update_sensor_data(); }