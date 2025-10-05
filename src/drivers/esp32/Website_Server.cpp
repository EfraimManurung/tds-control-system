#include "Website_Server.h"

Website_Server::Website_Server() {}
Website_Server::~Website_Server() {}

// Getter for motor circulation ON duration
unsigned int Website_Server::get_circulation_motor_time_on() const {
  return _circulation_motor_time_on;
}

// Getter for motor circulation OFF duration
unsigned int Website_Server::get_circulation_motor_time_off() const {
  return _circulation_motor_time_off;
}

// Checks if circulation motor is enabled
bool Website_Server::is_motor_enabled() const {
  return (_circulation_motor_state && *_circulation_motor_state);
}

// Getter for TDS set point
unsigned int Website_Server::get_tds_set_point() const {
  return (_tds_set_point) ? *_tds_set_point : 0;
}

unsigned int Website_Server::get_tds_calibration_value() const {
  return (_tds_calibration_value) ? *_tds_calibration_value : 0;
}

// Setup initializes pointers and states, sets up web endpoints
bool Website_Server::init(unsigned int motor_circulation_time_on,
                          unsigned int motor_circulation_time_off,
                          bool *motor_circulation_state, float *tds_set_point,
                          float *tds_calibration_value) {
  _circulation_motor_state = motor_circulation_state;
  _circulation_motor_time_on = motor_circulation_time_on;
  _circulation_motor_time_off = motor_circulation_time_off;
  _tds_set_point = tds_set_point;
  _tds_calibration_value = tds_calibration_value;

  // Register HTTP endpoints
  _server.on("/", HTTP_GET, [this]() { handle_root(); });
  _server.on("/set", HTTP_POST, [this]() { handle_set(); });
  _server.on("/off", HTTP_GET, [this]() { handle_off(); });

  _server.begin();

  return true;
}

// Generates the web UI with a form to configure motor circulation & TDS set
// point
void Website_Server::handle_root() {
  String html =
      R"rawliteral(
    <!DOCTYPE html><html>
    <head><title>TDS Control System</title></head>
    <body>
      <h1>TDS Control System</h1>
      <form action="/set" method="POST">
        <h2>Motor Circulation</h2>
        Time ON (ms): <input type="number" name="motor_circ_on" value=")rawliteral" +
      String(_circulation_motor_time_on) + R"rawliteral("><br>
        Time OFF (ms): <input type="number" name="motor_circ_off" value=")rawliteral" +
      String(_circulation_motor_time_off) + R"rawliteral("><br>
        Enable:
        <select name="motor_circ_enable">
          <option value="1")rawliteral" +
      (is_motor_enabled() ? " selected" : "") + R"rawliteral(>On</option>
          <option value="0")rawliteral" +
      (!is_motor_enabled() ? " selected" : "") + R"rawliteral(>Off</option>
        </select><br>

        <h2>TDS Control</h2>
        Set Point (ppm): <input type="number" name="tds_set_point" value=")rawliteral" +
      String(get_tds_set_point()) + R"rawliteral("><br>

        Calibration Value: <input type="number" step="0.1" name="tds_calibration_value" value=")rawliteral" +
      String(get_tds_calibration_value()) + R"rawliteral("><br>

        <input type="submit" value="Update">
      </form>

      <form action="/off" method="GET" style="margin-top:20px;">
        <input type="submit" value="Turn OFF Everything" style="background:red;color:white;">
      </form>
    </body>
    </html>
  )rawliteral";

  _server.send(200, "text/html", html);
}

// Parses submitted form and updates internal state
void Website_Server::handle_set() {
  if (_server.hasArg("motor_circ_on"))
    _circulation_motor_time_on = _server.arg("motor_circ_on").toInt();

  if (_server.hasArg("motor_circ_off"))
    _circulation_motor_time_off = _server.arg("motor_circ_off").toInt();

  if (_server.hasArg("motor_circ_enable") && _circulation_motor_state) {
    int val = _server.arg("motor_circ_enable").toInt();
    *_circulation_motor_state = (val == 1);
  }

  if (_server.hasArg("tds_set_point") && _tds_set_point) {
    *_tds_set_point = _server.arg("tds_set_point").toInt();
  }

  if (_server.hasArg("tds_calibration_value") && _tds_calibration_value)
    *_tds_calibration_value = _server.arg("tds_calibration_value").toFloat();

  _server.sendHeader("Location", "/", true);
  _server.send(302, "text/plain", "");
}

// Turns off motor circulation
void Website_Server::handle_off() {
  if (_circulation_motor_state) {
    *_circulation_motor_state = false;
  }

  _server.sendHeader("Location", "/", true);
  _server.send(302, "text/plain", "");
}

// Handles HTTP traffic
void Website_Server::loop() { _server.handleClient(); }