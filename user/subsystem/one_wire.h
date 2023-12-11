#ifndef ONE_WIRE_H
#define ONE_WIRE_H

#include "gpio.h"

typedef struct{
  IOPort port;
} OneWire;

void init_one_wire(OneWire *, bool_t);
void OneWire_Reset(OneWire *);
void OneWire_WriteBit(OneWire *ds_port, uint8_t bit);
uint8_t OneWire_ReadBit(OneWire *ds_port);
void OneWire_WriteByte(OneWire *ds_port, uint8_t byte);
uint8_t OneWire_ReadByte(OneWire *ds_port);
bool_t OneWire_SlaveWriteBit(OneWire *port, uint8_t bit);
uint8_t OneWire_SlaveReadBit(OneWire *port);
bool_t OneWire_SlaveWriteByte(OneWire *port, uint8_t byte);
bool_t OneWire_SlaveReadByte(OneWire *port, uint8_t *byte);
bool_t OneWire_SlaveDetectResetAndSendPresence(OneWire *port);

#endif
