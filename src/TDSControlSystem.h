#pragma once

/* class interface */
#include <interface/ActuatorInterface.h>
#include <interface/DisplayInterface.h>
#include <interface/GPIOInterface.h>
#include <interface/NetworkInterface.h>
#include <interface/SensorInterface.h>
#include <interface/StorageInterface.h>
#include <interface/WebsiteInterface.h>

/* specific sensor and actuator implementations */
#include <drivers/esp32/Actuator_L298N.h>
#include <drivers/esp32/Actuator_LED.h>
#include <drivers/esp32/Actuator_Manager.h>
#include <drivers/esp32/Actuator_Relay.h>
#include <drivers/esp32/GPIO_ESP32.h>
#include <drivers/esp32/Sensor_Manager.h>
#include <drivers/esp32/Sensor_TDS.h>

/* system libraries */
#include <systems/ISR_CircularBuffer.h>

// ======================
// L298N Motor Driver #0
// ======================

// Motor A (Left Motor)
#define ENA_MOTOR_0_PIN 13 // PWM - ENA
#define IN1_MOTOR_0_PIN 14 // Direction IN1
#define IN2_MOTOR_0_PIN 27 // Direction IN2

// Motor B (Right Motor)
#define ENB_MOTOR_1_PIN 33 // PWM - ENB
#define IN3_MOTOR_1_PIN 26 // Direction IN3
#define IN4_MOTOR_1_PIN 25 // Direction IN4

// =======================
// === RELAYS ===
// Three digital output pins to control relays
// =======================

#define RELAY_0_PIN 17 // OUTPUT - Relay control
#define RELAY_1_PIN 16 // OUTPUT - Relay control
#define RELAY_2_PIN 15 // OUTPUT - Relay control

// =======================
// === RGB LED ===
// 2-pin auto-cycling RGB LED: only needs ON/OFF control
// =======================

#define RGB_LED_PIN 2 // OUTPUT - ON/OFF control, also onboard LED pin

// =======================
// === OLED DISPLAY ===
// I2C interface using default pins on ESP32
// =======================

#define OLED_SDA_PIN 21 // I2C SDA line - OUTPUT
#define OLED_SCL_PIN 22 // I2C SCL line - OUTPUT

#define TDS_SENSOR_PIN 35

struct motor_timing {
  unsigned int time_on;
  unsigned int time_off;
};

class TDSControlSystem {

public:
  TDSControlSystem(WebsiteInterface &web, DisplayInterface &disp,
                   NetworkInterface &net, Sensor_Manager &sens_mgr,
                   Actuator_Manager &act_mgr, const char *wifi_ssid,
                   const char *wifi_password);

  bool init();

  /* for now it is only for elapsed time */
  bool update_sensor_data();

  void website_task(void *parameter);
  void circulation_motor_task(void *parameter);
  void update_circulation_motor_on_off_time();

private:
  WebsiteInterface &_web;
  DisplayInterface &_disp;
  NetworkInterface &_net;
  Sensor_Manager &_sens_mgr;
  Actuator_Manager &_act_mgr;

  const char *_wifi_ssid;
  const char *_wifi_password;

  /* to count elapsed time */
  unsigned long _start_millis;

  // time on and off for
  unsigned int _circulation_motor_time_on = 150000;
  unsigned int _circulation_motor_time_off = 150000;

  bool _motor_circulation_state = false;
  float _tds_set_point = 750.0;
  /**
   * based on TDS caliration buffer solution, with a TDS value of 500 ppm.
   *
   * Sample(s):
   *  - TDS sensor - TDS calibration buffer solution
   *  1. 715 - 500 = 215 ppm (offset)
   *  2. 710 - 500 = 210 ppm (offset)
   *  3. 706 - 500 = 206 ppm (offset)
   *
   * Sample with other TDS meters
   *  1. 780 - 531 = 249 ppm (offset)
   *
   * So the default offset is averaged values from above, and it is 880 / 4 =
   * 220 ppm
   *
   *  */
  float _tds_calibration_value = 220.0;

  Actuator_LED _led_on_board{RGB_LED_PIN};
  GPIO_ESP32 _gpio_esp32;

  Actuator_Relay _circulation_motor{RELAY_0_PIN};
  Actuator_L298N _dosing_motor{&_gpio_esp32, ENA_MOTOR_0_PIN, IN1_MOTOR_0_PIN,
                               IN2_MOTOR_0_PIN};

  bool _circulation_motor_states = false;
  motor_timing _circulation_motor_timings;
  unsigned long _last_circulation_motor_toggle;

  /* sensor */
  Sensor_TDS _tds_sensor{TDS_SENSOR_PIN};

  float _current_tds_value = 0.0;

  /* ISR */
  ISR_CircularBuffer<int, 8> _ISR_cb;  // global buffer
  hw_timer_t *_ISR_hw_timer = nullptr; // timer handle
  volatile int _ISR_counter = 0;

  /* static singleton pointer */
  static TDSControlSystem *instance;

  static void IRAM_ATTR _ISR_on_timer_wrapper(); // trampoline for ISR
  void IRAM_ATTR _ISR_on_timer();                // actual ISR
public:
  void display_task(void *pvParams); // constumer task - display task
};