#include "crc8.h"
#include <stdint.h>

#define POLYNOMIAL  0x7

uint8_t crc8_table[256] = {0};

void generate_crc8_table() {
  if (crc8_table[0] == 0 && crc8_table[255] == 0){ /* not inited */
    for (int i = 0; i < 256; i++) {
      unsigned char crc = i;
      for (int j = 0; j < 8; j++) {
        if (crc & 0x80) {
          crc = (crc << 1) ^ POLYNOMIAL;
        } else {
          crc <<= 1;
        }
      }
      crc8_table[i] = crc;
    }
  }
}

uint8_t compute_crc8(uint8_t *data, uint16_t length) {
  uint8_t crc = 0xFF;
  for (int i = 0; i < length; i++) {
    crc = crc8_table[crc ^ data[i]];
  }
  return crc;
}

