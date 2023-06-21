#ifndef __KEY_H
#define __KEY_H

#include "at32f403a_407.h"

#ifdef __cplusplus
extern "C" {
#endif

#define KEEPSTATUSTIME 5000

#define KEY_PORT                  GPIOC
#define KEY_02_ENTER_PIN          GPIO_PINS_14
#define KEY_01_SELECT_PIN         GPIO_PINS_15
#define KEY_CRM_CLK               CRM_GPIOC_PERIPH_CLOCK

typedef struct 
{        
  uint8_t    Dismode; //0:normal;1:Select,2:Enter
  uint8_t    Enter;   //Use to ADD
  uint16_t   KeyCnt10s;//keep status for 10s
} KeyPressMode;

void GPIO_Init(void);
void Key_ScanFun(void);
void DisplayPra(uint8_t Type);
#endif

