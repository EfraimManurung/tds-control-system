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

  // Add and setup actuators
  _act_mgr.add(&_led_on_board);
  _act_mgr.add(&_circulation_motor);
  _act_mgr.add(&_dosing_motor);

  if (!_act_mgr.init()) {
    Serial.println("Actuator Manager failed to initialize");
    return false;
  } else {
    Serial.println("Actuator Manager initialized");
  }

  _sens_mgr.add(&_tds_sensor);
  if (!_sens_mgr.init()) {
    Serial.println("Sensor Manager failed to initialize");
    return false;
  } else {
    Serial.println("Sensor Manager initialized");
  }

  _web.init(_circulation_motor_time_on, _circulation_motor_time_off,
            &_motor_circulation_state, &_tds_set_point);

  _disp.init();
  _disp.set_setted_tds_value(_tds_set_point);

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
  _current_tds_value = _tds_sensor.get_tds_value();
  _disp.set_current_tds_value(_current_tds_value);

  _tds_set_point = _web.get_tds_set_point();
  _disp.set_setted_tds_value(_tds_set_point);

  vTaskDelay(pdMS_TO_TICKS(100)); // Update every 100 ms, so it is not sluggish

  return true;
}

void TDSControlSystem::website_task(void *parameter) {
  TickType_t last_wake_time = xTaskGetTickCount();
  for (;;) {
    _web.loop();
    vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(10));
  }
}

void TDSControlSystem::circulation_motor_task(void *parameter) {
  TickType_t last_wake_time = xTaskGetTickCount();
  for (;;) {

    _circulation_motor_timings = {_web.get_circulation_motor_time_on(),
                                  _web.get_circulation_motor_time_off()};

    // Plasma control
    this->update_circulation_motor_on_off_time();

    vTaskDelayUntil(&last_wake_time, pdMS_TO_TICKS(10));
  }
}

void TDSControlSystem::update_circulation_motor_on_off_time() {
  unsigned long now = millis();
  unsigned int delay = _circulation_motor_states
                           ? _circulation_motor_timings.time_on
                           : _circulation_motor_timings.time_off;

  if (now - _last_circulation_motor_toggle >= delay) {
    _circulation_motor.set_state(_circulation_motor_states);
    _circulation_motor_states = !_circulation_motor_states;
    _led_on_board.set_state(_circulation_motor_states);
    _last_circulation_motor_toggle = now;
    Serial.printf("[TDSControlSystem] Circulation motor state changed to: %s\n",
                  _circulation_motor_states ? "ON" : "OFF");
  }
}