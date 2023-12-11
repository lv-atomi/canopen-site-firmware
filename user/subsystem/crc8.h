#ifndef CRC8_H
#define CRC8_H

#include <stdint.h>

void generate_crc8_table();
uint8_t compute_crc8(uint8_t *data, uint16_t length);

#endif
