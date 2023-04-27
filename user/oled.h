#ifndef __OLED_H
#define __OLED_H

#include "at32f403a_407.h"

#ifdef __cplusplus
extern "C" {
#endif

#define OLED_CMD  0	//Ð´ÃüÁî
#define OLED_DATA 1	//Ð´Êý¾Ý

#define OLED_EN_PORT        GPIOC
#define OLED_EN_PIN         GPIO_PINS_13
#define OLED_EN_CRM_CLK     CRM_GPIOC_PERIPH_CLOCK

#define OLED_SDA_PORT       GPIOA
#define OLED_SDA_PIN				GPIO_PINS_12
#define OLED_SDA_CRM_CLK    CRM_GPIOA_PERIPH_CLOCK

#define OLED_SCL_PORT       GPIOB
#define OLED_SCL_PIN        GPIO_PINS_7
#define OLED_SCL_CRM_CLK    CRM_GPIOB_PERIPH_CLOCK


#define OLED_EN_OFF()       gpio_bits_set(OLED_EN_PORT,OLED_EN_PIN)
#define OLED_EN_ON()        gpio_bits_reset(OLED_EN_PORT,OLED_EN_PIN)

#define OLED_SCL_Clr()      gpio_bits_reset(OLED_SCL_PORT,OLED_SCL_PIN)//SCL
#define OLED_SCL_Set()      gpio_bits_set(OLED_SCL_PORT,OLED_SCL_PIN)

#define OLED_SDA_Clr()      gpio_bits_reset(OLED_SDA_PORT,OLED_SDA_PIN)//DIN
#define OLED_SDA_Set()      gpio_bits_set(OLED_SDA_PORT,OLED_SDA_PIN)



void OLED_Init(void);
void OLED_Clear(void);
void OLED_Refresh(void);
void OLED_DisPlay_Off(void);
void OLED_DisPlay_On(void);
void OLED_ScrollDisplay(uint8_t num,uint8_t space,uint8_t mode);
void OLED_ShowNum(u8 x,u8 y,u32 num,u8 len,u8 size1,u8 mode);
void OLED_ShowChar(uint8_t x,uint8_t y,uint8_t chr,uint8_t size1,uint8_t mode);
void OLED_ShowString(uint8_t x,uint8_t y,uint8_t *chr,uint8_t size1,uint8_t mode);
void OLED_ShowChinese(uint8_t x,uint8_t y,uint8_t num,uint8_t size1,uint8_t mode);
void OLED_ShowPicture(uint8_t x,uint8_t y,uint8_t sizex,uint8_t sizey,uint8_t BMP[],uint8_t mode);

#endif

