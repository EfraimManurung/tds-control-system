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

#define RELAY_0_PIN 15 // OUTPUT - Relay control
#define RELAY_1_PIN 16 // OUTPUT - Relay control
#define RELAY_2_PIN 17 // OUTPUT - Relay control

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

class TDSControlSystem {

public:
  TDSControlSystem(WebsiteInterface &web, DisplayInterface &disp,
                   NetworkInterface &net, Sensor_Manager &sens_mgr,
                   Actuator_Manager &act_mgr, const char *wifi_ssid,
                   const char *wifi_password);

  bool init();

  /* for now it is only for elapsed time */
  bool update_sensor_data();

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
  unsigned int _circulation_motor_time_on = 60000;
  unsigned int _circulation_motor_time_off = 30000;

  bool _motor_circulation_state = false;
  unsigned int _tds_set_point = 750;
};