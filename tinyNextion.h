#ifndef TINY_NEXTION_H
#define TINY_NEXTION_H

#include <sys/types.h>
#include <SoftwareSerial.h>

#define NEXTION_BUFFER_SIZE                10
#define NEXTION_SERIAL_CYCLES              255

#define NEXTION_EVENT_RELEASE              0
#define NEXTION_EVENT_PRESS                1

#define NEXTION_CMD_STARTUP                0x00
#define NEXTION_CMD_TOUCH_EVENT            0x65
#define NEXTION_CMD_CURRENT_PAGE           0x66
#define NEXTION_CMD_TOUCH_COORDINATE_AWAKE 0x67
#define NEXTION_CMD_TOUCH_COORDINATE_SLEEP 0x68
#define NEXTION_CMD_STRING_DATA_ENCLOSED   0x70
#define NEXTION_CMD_NUMERIC_DATA_ENCLOSED  0x71
#define NEXTION_CMD_AUTO_ENTER_SLEEP       0x86
#define NEXTION_CMD_AUTO_ENTER_WAKEUP      0x87
#define NEXTION_CMD_READY                  0x88
#define NEXTION_CMD_START_MICROSD_UPDATE   0x89
#define NEXTION_CMD_TRANSPARENT_DATA_END   0xFD
#define NEXTION_CMD_TRANSPARENT_DATA_READY 0xFE

struct nextionComponent {
  int8_t page, id;
};

struct nextionEvent {
  int8_t page, id;
  bool state;
};

typedef void (*nextionOnChange) (bool);
typedef void (*nextionOnEvent) (uint8_t, uint8_t, bool);
typedef void (*nextionOnPointer) ();
typedef void (*nextionOnTouch) (uint16_t, uint16_t, bool);

class tinyNextion {
  protected:
    uint8_t *_buffer = (uint8_t *) malloc(NEXTION_BUFFER_SIZE);
    String _data = "";
    size_t _index = 0;
    size_t _length = 0;
    SoftwareSerial *_serial;
    uint8_t _signal = NEXTION_SERIAL_CYCLES;

  private:
    nextionOnTouch _onTouch;
    nextionOnEvent _onEvent;

    uint32_t baud();
    bool connect();
    void flush();
    uint8_t read();
    uint8_t readln();
    void restore();
    void send(String data);

  public:
    tinyNextion(uint8_t rx, uint8_t tx);
    uint32_t begin(uint32_t speed = 0);
    uint8_t backlight(uint8_t value);
    String get(String data);
    int16_t listen();
    void onEvent(nextionOnEvent pointer);
    void onTouch(nextionOnTouch pointer);
    int16_t page();
    uint8_t print(String data);
    uint32_t setBaud(uint32_t speed);
};

#endif
