#include "ad7705.h"
#include "at32f403a_407_board.h"
#include "gpio.h"
#include "spi.h"
#include "log.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define MAX_WAITING_DRDY  3000000	/* timeout: 3000ms */

uint8_t AD7705_WriteToReg(AD7705Port * devport, uint8_t byteWord) {
  /* 发送字节 */
  while (spi_i2s_flag_get(devport->controller, SPI_I2S_TDBE_FLAG) == RESET)
    ;
  spi_i2s_data_transmit(devport->controller, byteWord);
  while (spi_i2s_flag_get(devport->controller, SPI_I2S_RDBF_FLAG) == RESET)
    ;
  return spi_i2s_data_receive(devport->controller);	/* recv dummy data */
}

void ad7705ClockSetupSync(AD7705Port * devport,
			  AD7705_ClockDiv adClockDiv,
			  AD7705_MasterClock adMasterClock,
			  AD7705_UpdateRate adUpdateFrq,
			  AD7705_Channel channel) {
  gpio_set(&devport->nCS, RESET);
  AD7705_WriteToReg(devport, 0x20 | channel);
  AD7705_WriteToReg(devport, adClockDiv | adMasterClock | adUpdateFrq);
  gpio_set(&devport->nCS, SET);
}

void ad7705SetupAndCalibrateSync(AD7705Port *devport,
				 AD7705_Gain gain,
				 AD7705_SetupFlags flags,
				 AD7705_Channel channel) {
  gpio_set(&devport->nCS, RESET);
  AD7705_WriteToReg(devport, 0x10 | channel);
  AD7705_WriteToReg(devport, flags | ((gain & 0x07) << 3) | 0x40);
  gpio_set(&devport->nCS, SET);
}

void ad7705SetupNormalSync(AD7705Port *devport,
			   AD7705_Gain gain,
			   AD7705_SetupFlags flags,
                           AD7705_Channel channel) {
  gpio_set(&devport->nCS, RESET);
  AD7705_WriteToReg(devport, 0x10 | channel);
  AD7705_WriteToReg(devport, flags | ((gain & 0x07) << 3));
  gpio_set(&devport->nCS, SET);
}

uint16_t ad7705SPITransfer16Sync(AD7705Port *devport, uint16_t bDataOut) {
  uint8_t high;
  uint8_t low;

  high = (uint8_t)((bDataOut >> 8) & 0xFF);
  low  = (uint8_t)((bDataOut     ) & 0xFF);

  high = AD7705_WriteToReg(devport, high);
  low  = AD7705_WriteToReg(devport, low);

  return  ((((uint16_t)high) << 8) & 0xFF00) |  (((uint16_t)low) & 0x00FF);
}

uint16_t ad7705GetADCSync(AD7705Port *devport, AD7705_Channel ch) {
    uint16_t result;

    gpio_set(&devport->nCS, RESET);
    AD7705_WriteToReg(devport, 0x38 | ch); /* Or 0x39 if AD7705_CH2 should be selected */
    if (waiting_for_timeout(&devport->nDRDY, RESET, MAX_WAITING_DRDY)) { /* nonzero -> timeout */
      log_printf("timeout1\n");
      return 0;
    }
    
    result = ad7705SPITransfer16Sync(devport, 0xffff);
    gpio_set(&devport->nCS, SET);

    return result;
}


void ad7705ResetSoftSync(AD7705Port *devport) {
    gpio_set(&devport->nCS, RESET);
    
    AD7705_WriteToReg(devport, 0xFF);
    AD7705_WriteToReg(devport, 0xFF);
    AD7705_WriteToReg(devport, 0xFF);
    AD7705_WriteToReg(devport, 0xFF);

    gpio_set(&devport->nCS, SET);
}


/* uint16_t AD7705_ReadOne(AD7705Port * devport, int channel) { */
  
/*   /\* 等待/DRDY线低 *\/ */
/*   if (waiting_for_timeout(&devport->nDRDY, RESET, MAX_WAITING_DRDY)) { /\* nonzero -> timeout *\/ */
/*     log_printf("timeout1\n"); */
/*     return 0; */
/*   } */
/*   /\* while (gpio_read(&devport->nDRDY) == RESET)  ; *\/ */
  
/*   /\* 设置/CS线低 *\/ */
/*   gpio_set(&devport->nCS, RESET); */
/*   /\* choose data register, read *\/ */
/*   AD7705_WriteToReg(devport, 0x38 | channel); */


/*   uint16_t buffer = 0; */
/*   for (uint8_t b = 0; b < 2; b++) { */
/*     while(spi_i2s_flag_get(devport->controller, SPI_I2S_TDBE_FLAG) == RESET) */
/*       ; */
/*     spi_i2s_data_transmit(devport->controller, 0x58); /\* send dummy data, 0x58 indicate no operation *\/ */
    
/*     while(spi_i2s_flag_get(devport->controller, SPI_I2S_RDBF_FLAG) == RESET) */
/*       ; */
/*     uint16_t val = spi_i2s_data_receive(devport->controller); */
/*     buffer = (buffer << 8) | val; */
/*   } */
  
/*   /\* 等待/DRDY线高 *\/ */
/*   if (waiting_for_timeout(&devport->nDRDY, SET, MAX_WAITING_DRDY)) { /\* nonzero -> timeout *\/ */
/*     log_printf("timeout2\n"); */
/*     return 0; */
/*   } */
/*   /\* while (gpio_read(&devport->nDRDY) == SET) ;  *\/ */
  
/*   /\* 设置/CS线高 *\/ */
/*   gpio_set(&devport->nCS, SET); */

/*   return buffer; */
/* } */

/* uint16_t * AD7705_ReadDualChannel(AD7705Port * devport) { */
/*   static uint16_t buffer[2]; */
/*   uint8_t i; */
/*   for (i=0; i<2; i++){ */
/*     buffer[i] = AD7705_ReadOne(devport, i); */
/*   } */
/*   return buffer; */
/* } */

int32_t ConvertToDegree(uint16_t value){
  int32_t r = abs(value - 32767);
  r = r * 500 / 32 * 100 / 65536;
  return r;
}


/* void AD7705_Init(AD7705Port * devport, uint8_t channel){ */
/*   uint8_t i; */
/*   /\* 设置/CS线低 *\/ */
/*   gpio_set(&devport->nCS, RESET); */
/*   for(i=0; i<150; i++) AD7705_WriteToReg(devport, 0xff); */
/*   delay_ms(1); */
    
/*   channel == 0 ? AD7705_WriteToReg(devport, 0x20): AD7705_WriteToReg(devport, 0x21); /\* clock register, channel 0 or 1 *\/ */

/*   AD7705_WriteToReg(devport, 0b01100);	/\* Set main clock, CLKDIS:0->Master clock enabled, */
/* 					   CLKDIV:0-> no dividison; 1-> divided by 2 */
/* 					   CLK:1-> USE 2Mhz(with clkdiv), */
/* 					   FS[1..0]:00->20Hz Rate *\/ */
  

/*   channel == 0 ? AD7705_WriteToReg(devport, 0x10): AD7705_WriteToReg(devport, 0x11); /\* setup register, channel 0 or 1 *\/ */

/*   /\* setup register, MD[1..0]:01-> self-calibration, G[2..0]:011->x8 gain, */
/*      nB/U:1->Unipolar, 0->bipolar, */
/*      BUF:1-> buffer mode, FSYNC:0->process data *\/ */

/*   AD7705_WriteToReg(devport, 0b01011000); */
/*   if (waiting_for_timeout(&devport->nDRDY, RESET, MAX_WAITING_DRDY)){ */
/*     log_printf("timeout3\n"); */
/*   } */
/*   /\* /\\* 等待/DRDY线低 *\\/ *\/ */
/*   /\* while(gpio_read(&devport->nDRDY) == SET); *\/ */
/*   /\* 设置/CS线高 *\/ */
/*   gpio_set(&devport->nCS, SET); */
/* } */

void init_ad7705(AD7705Port * devport){
  ASSERT(devport);
  memset((void*)&devport->_spi, 0, sizeof(SPIPort));
  init_gpio_output(&devport->reset, GPIO_OUTPUT_PUSH_PULL, GPIO_DRIVE_STRENGTH_STRONGER);
  /* init_gpio_output(&devport->nCS, GPIO_OUTPUT_PUSH_PULL, GPIO_DRIVE_STRENGTH_STRONGER); */
  init_gpio_input(&devport->nDRDY, GPIO_PULL_NONE, GPIO_DRIVE_STRENGTH_MODERATE);

  devport->_spi.clk = devport->clk;
  devport->_spi.cs = devport->nCS;
  devport->_spi.miso = devport->dout;
  devport->_spi.mosi = devport->din;
  devport->_spi.controller = devport->controller;
  
  devport->_spi.init_type.clock_phase = SPI_CLOCK_PHASE_2EDGE;
  devport->_spi.init_type.clock_polarity = SPI_CLOCK_POLARITY_HIGH;
  devport->_spi.init_type.cs_mode_selection = SPI_CS_SOFTWARE_MODE;
  devport->_spi.init_type.frame_bit_num = SPI_FRAME_8BIT;
  devport->_spi.init_type.first_bit_transmission = SPI_FIRST_BIT_MSB;
  devport->_spi.init_type.mclk_freq_division = SPI_MCLK_DIV_64;
  devport->_spi.init_type.master_slave_mode = SPI_MODE_MASTER;
  devport->_spi.init_type.transmission_mode = SPI_TRANSMIT_FULL_DUPLEX;
  init_spi(&devport->_spi);

  /* reset ad7705 */
  gpio_set(&devport->nCS, RESET);
  gpio_set(&devport->reset, RESET); delay_ms(1);

  /* send two dummy bytes */
  while(spi_i2s_flag_get(devport->controller, SPI_I2S_TDBE_FLAG) == RESET) ;
  spi_i2s_data_transmit(devport->controller, 0x58); /* send dummy data, 0x58 indicate no operation */
  while(spi_i2s_flag_get(devport->controller, SPI_I2S_RDBF_FLAG) == RESET) ;
  spi_i2s_data_receive(devport->controller); /* recv dummy data */

  while(spi_i2s_flag_get(devport->controller, SPI_I2S_TDBE_FLAG) == RESET);
  spi_i2s_data_transmit(devport->controller, 0x58); /* send dummy data, 0x58 indicate no operation */
    
  while(spi_i2s_flag_get(devport->controller, SPI_I2S_RDBF_FLAG) == RESET);
  spi_i2s_data_receive(devport->controller); /* recv dummy data */

  gpio_set(&devport->reset, SET); delay_ms(1);
  gpio_set(&devport->nCS, SET); delay_ms(1);

  /* AD7705_Init(devport, 0); delay_ms(1); */
  /* AD7705_Init(devport, 1); delay_ms(1); */
  /* ad7705_setup(devport, AD7705_CH1); */
  /* ad7705_setup(devport, AD7705_CH2); */
}

void ad7705_setup(AD7705Port * devport, AD7705_Channel ch){
  ad7705ResetSoftSync(devport);
  ad7705ClockSetupSync(devport,
		       AD7705_CLOCKDIV_2,
		       AD7705_MASTERCLK_MHZ2,
		       AD7705_UPDATE_HZ50,
		       ch);
  ad7705SetupAndCalibrateSync(devport, AD7705_GAIN_8,
			      AD7705_SETUP_BUFFER_ENABLE | AD7705_SETUP_BIPOLAR,
			      ch);
}

void ad7705_irq_handler(int spi_index) {}
