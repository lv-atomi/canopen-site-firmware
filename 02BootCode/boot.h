#ifndef __BOOT_H
#define __BOOT_H

#include "at32f403a_407.h"

#ifdef __cplusplus
extern "C" {
#endif

#define BOOTWAITTIME 100000

void Init_Ram(void);
void app_clear_sys_status(void);
void boot_jump_to_app(void);
void app_jump_address_set(void);
void app_jump(void);
#endif

