#ifndef __FLASH_H__
#define __FLASH_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "at32f403a_407.h"


#define SECTOR_SIZE                      2048   /* this parameter depends on the specific model of the chip */

#define TEST_BUFEER_SIZE                 2048
#define TEST_FLASH_ADDRESS_START         0X804B000//(0x08000000 + 1024 * 1020)//1020:0X80FF000//20:0X08005000

void flash_read(uint32_t read_addr, uint16_t *p_buffer, uint16_t num_read);
error_status flash_write_nocheck(uint32_t write_addr, uint16_t *p_buffer, uint16_t num_write);
error_status flash_write(uint32_t write_addr,uint16_t *p_Buffer, uint16_t num_write);


#ifdef __cplusplus
}
#endif

#endif

















