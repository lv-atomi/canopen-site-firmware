#ifndef __OLED_H
#define __OLED_H

#include "gpio.h"
#include "board.h"
#include "i2c.h"
#include <stdint.h>

#define OLED_GRAM_MAX_Y 5
#define OLED_GRAM_MAX_X 88


typedef struct{
  IOPort en;
  I2CPort i2c;
  uint8_t mode_padding;		/* declare 1 byte ahead of OLED_GRAM used for indicate oled_mode*/
  uint8_t OLED_GRAM[OLED_GRAM_MAX_Y][OLED_GRAM_MAX_X];
} OLEDPort;

void init_oled(OLEDPort *devport);
void oled_clear(OLEDPort *devport);
void oled_display_off(OLEDPort *devport);
void oled_display_on(OLEDPort *devport);
void oled_showchar(OLEDPort *devport, uint8_t x, uint8_t y, char chr, uint8_t size1, uint8_t mode);
void oled_showstring(OLEDPort *devport, uint8_t x, uint8_t y, char *str, uint16_t strlen,
		     uint8_t font_size, uint8_t mode);
void oled_refresh(OLEDPort *devport);

#endif
