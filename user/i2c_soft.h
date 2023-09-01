#ifndef I2C_SOFT_H
#define I2C_SOFT_H
#include "i2c.h"

void init_i2c_soft(I2CPort *port);
void i2c_master_transmit_soft(I2CPort *port, uint8_t *pdata, uint16_t size,
                              uint32_t timeout);

#endif
