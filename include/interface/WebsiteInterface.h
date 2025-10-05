#pragma once

#include <stdint.h>

class WebsiteInterface {
public:
  virtual ~WebsiteInterface() = default;

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
  virtual bool init(unsigned int motor_circulation_time_on,
                    unsigned int motor_circulation_time_off,
                    bool *motor_circulation_state, float *tds_set_point,
                    float *tds_calibration_value) = 0;

  // Main loop to handle HTTP server events
  virtual void loop() = 0;

  // Getter for motor circulation ON duration
  virtual unsigned int get_circulation_motor_time_on() const = 0;

  // Getter for motor circulation OFF duration
  virtual unsigned int get_circulation_motor_time_off() const = 0;

  // Checks if circulation motor is enabled
  virtual bool is_motor_enabled() const = 0;

  // Getter for TDS set point
  virtual unsigned int get_tds_set_point() const = 0;

  // Getter for TDS calibration value
  virtual unsigned int get_tds_calibration_value() const = 0;
};