#ifndef __BOOT_H
#define __BOOT_H

#include "at32f403a_407.h"

#ifdef __cplusplus
extern "C" {
#endif

void app_clear_sys_status(void);
void app_jump_to_bootloader(void);
void app_jump_address_set(void);
#endif

