/**
 *
 * Total Dissolved Solids (TDS) Control System
 *
 * Author: Efraim Manurung
 * Email: efraim.manurung@gmail.com
 *
 * Changelog:
 *
 * [29-09-2025]
 * - Added circular buffer example, TDS sensor, and refactored some code and
 * control flow.
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

// === Module Declarations ===
Actuator_Manager act_mgr; // Manages all actuators (LED, Motors, Relays)
Display_SSD1306 ssd1306;  // OLED display driver
Sensor_Manager
    sens_mgr; // Manages all sensors (keypads, buttons, water level, etc)

// === WiFi Configuration ===
#ifdef USE_LOCAL_WIFI
#define DEFAULT_SSID "PLASMART-Access-Point"
#define DEFAULT_PASS "plasmart1123"

Local_WiFi wifi;
#else
#define DEFAULT_SSID "Manurung_1"
#define DEFAULT_PASS "zephan23"

Network_WiFi wifi; // WiFi connectivity manager
#endif

Website_Server web; // Web interface for configuration and monitoring

// === Core System Instance ===
TDSControlSystem tds_cs(web, ssd1306, wifi, sens_mgr, act_mgr, DEFAULT_SSID,
                        DEFAULT_PASS);

// === Task Trampolines ===
// These allow passing class methods to C-style FreeRTOS tasks.
void website_task_trampoline(void *param) { tds_cs.website_task(param); }

void circulation_motor_task_trampolin(void *param) {
  tds_cs.circulation_motor_task(param);
}

void setup() {
  Serial.begin(115200);

  tds_cs.init();

  // Web server handler (LOW priority, runs on core 0)
  xTaskCreatePinnedToCore(website_task_trampoline, // function
                          "WebTask",               // name
                          4096,                    // stack size
                          NULL,                    // param
                          2,                       // priority
                          NULL,                    // handle
                          0                        // core
  );

  // Plasma relay toggling (MEDIUM priority, core 1)
  xTaskCreatePinnedToCore(circulation_motor_task_trampolin,
                          "CirculationMotorTask", 4096, NULL, 2, NULL, 1);
}

void loop() { tds_cs.update_sensor_data(); }