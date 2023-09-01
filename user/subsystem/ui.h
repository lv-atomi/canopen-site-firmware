#ifndef UI_H
#define UI_H
#include "gpio.h"

typedef void (*store_id_cb)(uint8_t);
typedef uint8_t (*get_id_cb)(void);
typedef void (*store_timeout_cb)(uint16_t);
typedef uint16_t (*get_timeout_cb)(void);

void init_ui(store_id_cb, get_id_cb, store_timeout_cb, get_timeout_cb);
bool_t ui_tick(void);

#endif
