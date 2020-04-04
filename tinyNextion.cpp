#include "tinyNextion.h"

tinyNextion::tinyNextion(uint8_t rx, uint8_t tx) {
  _serial = new SoftwareSerial(rx, tx);
}

uint32_t tinyNextion::begin(uint32_t speed) {
  uint32_t rate = baud();
  if (speed && (speed != rate)) return setBaud(speed);
  return rate;
}

uint8_t tinyNextion::backlight(uint8_t value) {
  return print("dim=" + String(value));
}

uint32_t tinyNextion::baud() {
  const uint8_t map[8] = {48, 24, 16, 8, 4, 2, 1, 0};
  uint8_t index = 0;

  do _serial->begin(map[index] * 2400UL);
  while (!connect() && (7 > ++index));

  return map[index] * 2400UL;
}

bool tinyNextion::connect() {
  flush();
  send("DRAKJHSUYDGBNCJHGJKSHBDN");
  send("connect");
  send(String(char(0xFF) + char(0xFF)) + "connect");
  flush();
  send("");
  flush();
  send("connect");

  if (readln()) return _data.indexOf("comok") != -1;
}

void tinyNextion::flush() {
  _signal = NEXTION_SERIAL_CYCLES;
  do while (_serial->available()) {
      _serial->read();
      _signal = NEXTION_SERIAL_CYCLES;
    } while (_signal--);
}

String tinyNextion::get(String data) {
  switch (print("get " + data)) {

    case NEXTION_CMD_STRING_DATA_ENCLOSED:
      return _data.substring(0, _data.length() - 3);

    case NEXTION_CMD_NUMERIC_DATA_ENCLOSED:
      return String((uint32_t(_buffer[4]) << 24) + (uint32_t(_buffer[3]) << 16) + (uint32_t(_buffer[2]) << 8) + uint8_t(_buffer[1]));
  }
  return String(_buffer[0]);
}

int16_t tinyNextion::listen() {
  if (_serial->available() > 3)
    if (read()) {
      uint16_t data = int16_t(_buffer[0]);
      switch (data) {

        case NEXTION_CMD_STARTUP:
          break;

        case NEXTION_CMD_TOUCH_COORDINATE_AWAKE:
        case NEXTION_CMD_TOUCH_COORDINATE_SLEEP:
          if (_onTouch) _onTouch((uint16_t(_buffer[1]) << 8) | uint8_t(_buffer[2]), (uint16_t(_buffer[3]) << 8) | uint8_t(_buffer[4]), _buffer[5]);
          break;

        case NEXTION_CMD_TOUCH_EVENT:
          if (_onEvent) _onEvent(_buffer[1], _buffer[2], _buffer[3]);
          break;

        case NEXTION_CMD_AUTO_ENTER_SLEEP:
        case NEXTION_CMD_AUTO_ENTER_WAKEUP:
          break;

        case NEXTION_CMD_READY:
          break;

        case NEXTION_CMD_START_MICROSD_UPDATE:
          break;
      }
      return data;
    }
  return -1;
}

void tinyNextion::onEvent(nextionOnEvent pointer) {
  _onEvent = pointer;
}

void tinyNextion::onTouch(nextionOnTouch pointer) {
  _onTouch = pointer;
}

int16_t tinyNextion::page() {
  if (print("sendme") == NEXTION_CMD_CURRENT_PAGE) return uint8_t(_buffer[1]);
  else return -1;
}

uint8_t tinyNextion::print(String data) {
  flush();
  send(data);
  if (read()) return _buffer[0];
}

uint8_t tinyNextion::read() {
  uint8_t exit = 0;
  restore();
  do if (_serial->available()) {
      uint8_t data = uint8_t(_serial->read());
      _signal = NEXTION_SERIAL_CYCLES;

      switch (_buffer[0]) {

        case NEXTION_CMD_STRING_DATA_ENCLOSED:
          _data += char(data);
          _length = 1;
          exit = (data == 0xFF) * (exit + 1);
          break;

        case NEXTION_CMD_NUMERIC_DATA_ENCLOSED:
          _buffer[_length++] = uint8_t(data);
          exit = (_length == 8) * 3;
          break;

        default:
          _buffer[_length++] = uint8_t(data);
          exit = (data == 0xFF) * (exit + 1);
      }
    } while (_signal-- && (exit != 3));
  return _length;
}

uint8_t tinyNextion::readln() {
  restore();
  do while (_serial->available()) {
      _data += char(_serial->read());
      _signal = NEXTION_SERIAL_CYCLES;
    } while (_signal--);
  return _data.length();
}

void tinyNextion::restore() {
  _buffer[0] = 0x00;
  _data = "";
  _length = 0;
  _signal = NEXTION_SERIAL_CYCLES;
}

void tinyNextion::send(String data) {
  _serial->print(data + char(0xFF) + char(0xFF) + char(0xFF));
}

uint32_t tinyNextion::setBaud(uint32_t speed) {
  flush();
  send("baud=" + String(speed));
  return baud();
}
