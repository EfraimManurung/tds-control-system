/**
 *
 * Total Dissolved Solids (TDS) Control System
 *
 * Author: Efraim Manurung
 * Email: efraim.manurung@gmail.com
 *
 * Changelog:
 *
 * [05-10-2025]
 * - Changed some methods name, added getter for the calibration value in apps
 * and website objects.
 *
 * [01-10-2025]
 * - Added and implemented circular buffer in the main apps and TDSControlSystem
 * class.
 *
 * [29-09-2025]
 * - Added circular buffer example, TDS sensor, and refactored some code and
 * control flow.
 *
 * [28-09-2025]
 * -  Added basic, sensors and actuators classes and basic functions of the main
 * app.
 *
 * [25-09-2025]
 *  - First initialization.
 */

#include "TDSControlSystem.h"
#include <Arduino.h>

/* === Drivers === */
#include <drivers/esp32/Actuator_Manager.h>
#include <drivers/esp32/Display_SSD1306.h>
#include <drivers/esp32/Sensor_Manager.h>
#include <drivers/esp32/Website_Server.h>

#ifdef USE_LOCAL_WIFI
#include <drivers/esp32/Local_WiFi.h>
#else
#include <drivers/esp32/Network_WiFi.h>
#endif

/* === Global Managers === */
Actuator_Manager act_mgr; // Centralized actuator manager (LEDs, motors, relays)
Display_SSD1306 ssd1306;  // OLED display driver
Sensor_Manager sens_mgr;  // Centralized sensor manager (TDS, buttons, etc.)

/* === WiFi Configuration === */
#ifdef USE_LOCAL_WIFI
#define DEFAULT_SSID "TDS-System-Access-Point"
#define DEFAULT_PASS "tdscontrol1123"
Local_WiFi wifi;
#else
#define DEFAULT_SSID "Manurung_1"
#define DEFAULT_PASS "zephan23"
Network_WiFi wifi; // Default: STA mode WiFi manager
#endif

/* === Web Interface === */
Website_Server web; // HTTP server for monitoring/configuration

/* === Core Control System === */
TDSControlSystem tds_cs(web, ssd1306, wifi, sens_mgr, act_mgr, DEFAULT_SSID,
                        DEFAULT_PASS);

/* === FreeRTOS Task Trampolines ===
   FreeRTOS requires C-style function pointers,
   so these wrappers forward to class methods. */
void website_task_trampoline(void *param) { tds_cs.website_task(param); }
void circulation_motor_task_trampoline(void *param) {
  tds_cs.circulation_motor_task(param);
}
void display_task_trampoline(void *param) { tds_cs.display_task(param); }

/* === Arduino Setup === */
void setup() {
  Serial.begin(115200);
  Serial.println("\n[TDSControlSystem] Booting system...");

  // Initialize core control system (network, sensors, actuators, ISR timer)
  if (!tds_cs.init()) {
    Serial.println("[TDS] Initialization failed. Halting.");
    while (true) {
      delay(1000);
    } // fail-safe lockup
  }

  /* Spawn RTOS tasks with appropriate priorities and core affinity */

  // Web server handler (low priority → non-time-critical, core 0)
  xTaskCreatePinnedToCore(website_task_trampoline, "WebTask", 4096, nullptr, 2,
                          nullptr, 0);

  // Circulation motor relay control (medium priority, core 1)
  xTaskCreatePinnedToCore(circulation_motor_task_trampoline,
                          "CirculationMotorTask", 4096, nullptr, 2, nullptr, 1);

  // ISR consumer task (high priority, core 1)
  xTaskCreatePinnedToCore(display_task_trampoline, "DisplayTask", 4096, nullptr,
                          3, nullptr, 1);
}

/* === Arduino Loop ===
   Kept lightweight: only periodic sensor updates. */
void loop() { tds_cs.update_sensor_data(); }
