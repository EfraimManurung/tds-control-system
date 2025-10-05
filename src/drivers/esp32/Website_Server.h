// Website.h

#pragma once

#include <Arduino.h>
#include <WebServer.h>
#include <interface/WebsiteInterface.h>

class Website_Server : public WebsiteInterface {
public:
  Website_Server();
  ~Website_Server();

  /**
   * Initializes the web server with control settings and hardware states.
   *
   * @param motor_circulation_time_on   Duration (ms) the motor circulation
   * stays ON
   * @param motor_circulation_time_off  Duration (ms) the motor circulation
   * stays OFF
   * @param motor_circulation_state     Pointer to motor circulation ON/OFF
   * state
   * @param tds_set_point               Pointer to TDS set point value (shared
   * with main app)
   * @param tds_calibration_value       Pointer to TDS calibration value (shared
   * with main app)
   * @return                            true on success
   */
  bool init(unsigned int motor_circulation_time_on,
            unsigned int motor_circulation_time_off,
            bool *motor_circulation_state, float *tds_set_point,
            float *tds_calibration_value);

  // Main loop to handle HTTP server events
  void loop();

  // Getter for motor circulation ON duration
  unsigned int get_circulation_motor_time_on() const;

  // Getter for motor circulation OFF duration
  unsigned int get_circulation_motor_time_off() const;

  // Checks if circulation motor is enabled
  bool is_motor_enabled() const;

  // Getter for TDS set point
  unsigned int get_tds_set_point() const;

  unsigned int get_tds_calibration_value() const;

private:
  WebServer _server{80}; // Web server on port 80

  // External pointer to be manipulated via HTTP controls
  bool *_circulation_motor_state = nullptr; // circulation motor ON/OFF

  // Circulation motor ON/OFF timing in milliseconds
  unsigned int _circulation_motor_time_on;
  unsigned int _circulation_motor_time_off;

  // Pointer to TDS set point (shared with main app)
  float *_tds_set_point = nullptr;
  float *_tds_calibration_value = nullptr;

  // Web handlers for HTTP endpoints
  void handle_root();
  void handle_set();
  void handle_off();
};