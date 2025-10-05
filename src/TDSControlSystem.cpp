#include "TDSControlSystem.h"

// define the static instance pointer
TDSControlSystem *TDSControlSystem::instance = nullptr;

TDSControlSystem::TDSControlSystem(
    WebsiteInterface &web, DisplayInterface &disp, NetworkInterface &net,
    Sensor_Manager &sens_mgr, Actuator_Manager &act_mgr, const char *wifi_ssid,
    const char *wifi_password)
    : _web(web), _disp(disp), _net(net), _sens_mgr(sens_mgr), _act_mgr(act_mgr),
      _wifi_ssid(wifi_ssid), _wifi_password(wifi_password) {
  // Set singleton instance so the ISR wrapper can call
  // instance->_ISR_on_timer()
  instance = this;
}

bool TDSControlSystem::init() {

  /* setup hardware timer (0, prescaler 80 -> 1 tick = 1 us) */
  _ISR_hw_timer = timerBegin(0, 80, true);
  timerAttachInterrupt(_ISR_hw_timer, &TDSControlSystem::_ISR_on_timer_wrapper,
                       true);
  timerAlarmWrite(_ISR_hw_timer, 500000, true); // 500ms
  timerAlarmEnable(_ISR_hw_timer);

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
            &_motor_circulation_state, &_tds_set_point,
            &_tds_calibration_value);

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

  _tds_set_point = _web.get_tds_set_point();
  _disp.set_setted_tds_value(_tds_set_point);

  _tds_calibration_value = _web.get_tds_calibration_value();

  /* read sensor data */
  EventBits_t bits =
      xEventGroupWaitBits(_ISR_cb.event_group, EVENT_BIT_SENSOR_AVAILABLE,
                          pdTRUE,  // clear on exit
                          pdFALSE, // wait for any bit
                          portMAX_DELAY);

  if (bits & EVENT_BIT_SENSOR_AVAILABLE) {
    /* add the calibration in here */
    _current_tds_value = _tds_sensor.get_tds_value() + _tds_calibration_value;
    _ISR_cb.push(static_cast<int>(_current_tds_value));
  }

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

    // relay control
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

void IRAM_ATTR TDSControlSystem::_ISR_on_timer_wrapper() {
  if (instance) { // instance = global/static pointer to the singleton
    instance->_ISR_on_timer();
  }
}

void IRAM_ATTR TDSControlSystem::_ISR_on_timer() {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xEventGroupSetBitsFromISR(_ISR_cb.event_group, EVENT_BIT_SENSOR_AVAILABLE,
                            &xHigherPriorityTaskWoken);
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

void TDSControlSystem::display_task(void *pvParams) {
  for (;;) {
    EventBits_t bits =
        xEventGroupWaitBits(_ISR_cb.event_group, EVENT_BIT_DATA_AVAILABLE,
                            pdTRUE,  // clear on exit
                            pdFALSE, // wait for any bit
                            portMAX_DELAY);

    if (bits & EVENT_BIT_DATA_AVAILABLE) {
      while (!_ISR_cb.is_empty()) {
        int val = _ISR_cb.pop();
        // Serial.printf("TDS VALUES: %d\n", val);
        _disp.set_current_tds_value(val);
      }
    }

    if (bits & EVENT_BIT_BUFFER_FULL) {
      Serial.println("⚠️ Buffer full (oldest overwritten)");
    }
  }
}