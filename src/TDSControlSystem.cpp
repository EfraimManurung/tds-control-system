#include "TDSControlSystem.h"

TDSControlSystem::TDSControlSystem(
    WebsiteInterface &web, DisplayInterface &disp, NetworkInterface &net,
    Sensor_Manager &sens_mgr, Actuator_Manager &act_mgr, const char *wifi_ssid,
    const char *wifi_password)
    : _web(web), _disp(disp), _net(net), _sens_mgr(sens_mgr), _act_mgr(act_mgr),
      _wifi_ssid(wifi_ssid), _wifi_password(wifi_password) {}

bool TDSControlSystem::init() {
  /* start time to count the elapsed time */
  _start_millis = millis();

  /* init */
  if (!_net.init(_wifi_ssid, _wifi_password)) {
    Serial.println("Network init failed!");
    return false;
  }

  _web.init(_circulation_motor_time_on, _circulation_motor_time_off,
            &_motor_circulation_state, &_tds_set_point);

  _disp.init();

  // Display IP address
  _disp.set_ip_address(_net.get_ip_address());

  return true;
}

bool TDSControlSystem::update_sensor_data() {
  unsigned long now = millis();
  unsigned long elapsed = (now - _start_millis) / 1000; // seconds

  char timeStr[9]; // "HH:MM:SS"
  snprintf(timeStr, sizeof(timeStr), "%02lu:%02lu:%02lu", elapsed / 3600,
           (elapsed % 3600) / 60, elapsed % 60);

  _disp.set_elapsed_time(std::string(timeStr));

  vTaskDelay(pdMS_TO_TICKS(100)); // Update every 100 ms, so it is not sluggish

  return true;
}
