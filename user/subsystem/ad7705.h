#ifndef AD7705_H
#define AD7705_H

#include "gpio.h"
#include "spi.h"
#include "board.h"
#include <stdint.h>

typedef struct{
  IOPort nCS;
  IOPort nDRDY;
  IOPort clk;
  IOPort din;
  IOPort dout;
  IOPort reset;
  
  spi_type * controller;
  SPIPort _spi;
  int32_t value1;
  int32_t value2;
} AD7705Port;

typedef enum {
    AD7705_UPDATE_HZ20 = 0x00,
    AD7705_UPDATE_HZ25,
    AD7705_UPDATE_HZ100,
    AD7705_UPDATE_HZ200,
    AD7705_UPDATE_HZ50,
    AD7705_UPDATE_HZ60,
    AD7705_UPDATE_HZ250,
    AD7705_UPDATE_HZ500
} AD7705_UpdateRate;

typedef enum {
    AD7705_GAIN_1 = 0b000 << 3,
    AD7705_GAIN_2 = 0b001 << 3,
    AD7705_GAIN_4 = 0b010 << 3,
    AD7705_GAIN_8   = 0b011 << 3,
    AD7705_GAIN_16  = 0b100 << 3,
    AD7705_GAIN_32  = 0b101 << 3,
    AD7705_GAIN_64  = 0b110 << 3,
    AD7705_GAIN_128 = 0b111 << 3,
} AD7705_Gain;

typedef enum {
    AD7705_CLOCKDIV_1 = 0x00,
    AD7705_CLOCKDIV_2 = 0x08,
} AD7705_ClockDiv;

typedef enum {
    AD7705_MASTERCLK_MHZ1 = 0x00,
    AD7705_MASTERCLK_MHZ2 = 0x80,
    AD7705_MASTERCLK_MHZ24576 = 0x40,
    AD7705_MASTERCLK_MHZ49152 = 0xC0
} AD7705_MasterClock;

typedef enum {
    AD7705_CH1 = 0x00,
    AD7705_CH2 = 0x01
} AD7705_Channel;

typedef enum {
    AD7705_SETUP_BUFFER_ENABLE = 0x02,
    AD7705_SETUP_BUFFER_DISABLE = 0x00,
    AD7705_SETUP_UNIPOLAR = 0x04,
    AD7705_SETUP_BIPOLAR = 0x00,
    AD7705_SYNCHRONIZE = 0x01
} AD7705_SetupFlags;

void init_ad7705(AD7705Port * devport);
/* uint16_t AD7705_ReadOne(AD7705Port * devport, int channel); */
/* uint16_t * AD7705_ReadDualChannel(AD7705Port * devport); */
int32_t ConvertToDegree(uint16_t value);
void ad7705ClockSetupSync(AD7705Port * devport,
			  AD7705_ClockDiv adClockDiv,
			  AD7705_MasterClock adMasterClock,
			  AD7705_UpdateRate adUpdateFrq,
			  AD7705_Channel channel);
void ad7705SetupAndCalibrateSync(AD7705Port *devport,
				 AD7705_Gain gain,
				 AD7705_SetupFlags flags,
				 AD7705_Channel channel);
void ad7705SetupNormalSync(AD7705Port *devport,
			   AD7705_Gain gain,
			   AD7705_SetupFlags flags,
                           AD7705_Channel channel);
uint16_t ad7705GetADCSync(AD7705Port *devport, AD7705_Channel ch);
void ad7705ResetSoftSync(AD7705Port *devport);
void ad7705_irq_handler(int spi_index);
void ad7705_setup(AD7705Port * devport, AD7705_Channel ch);

#endif
