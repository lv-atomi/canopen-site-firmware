#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include "gpio.h"

#define KEYBOARD_MAX_KEYS  16
#define SCHMITT_TRIGGER_PRESS 2
#define SCHMITT_TRIGGER_RELEASE 2
#define REPEATIVE_TRIGGER_CYCLE 250

enum KEYSTATUS{
  KEY_PRESS,			/* key pressed */
  KEY_RELEASE,			/* key released */
  KEY_CLICKED,			/* pressed->release => key clicked */
  KEY_LONGPRESS,		/* pressed for long time => key longpress and send repeatively, durtion defined by REPEATIVE_TRIGGER_CYCLE */
  KEY_DUMMY			/* nothing happened */
};

enum KEY_CAPTURE{
  KEY_ACTIVE_HIGH,
  KEY_ACTIVE_LOW
};

typedef struct {
  IOPort port;
  uint8_t key_last_val;
  uint8_t restart;		/* 0=> nothing, 1=> waiting for key press down only*/
  uint16_t count_down;
  enum KEY_CAPTURE capture;
} KeyboardPort;

void init_keyboard(KeyboardPort * devport);
void keyboard_tick();		/* called every 1ms */
bool_t keyboard_get_event(KeyboardPort **triggered_port, enum KEYSTATUS * status);
void keyboard_suppress_longpress(KeyboardPort *port);

#endif
