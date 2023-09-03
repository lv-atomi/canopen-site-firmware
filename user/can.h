#ifndef __CAN_H
#define __CAN_H

#include "at32f403a_407.h"
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//#define BAUD_1M
#define BAUD_500K

void can_gpio_config(void);
void can_configuration(void);
void canopen_init();


#ifdef __cplusplus
}
#endif

#endif
