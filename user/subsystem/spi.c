#include "spi.h"
#include "at32f403a_407.h"
#include "at32f403a_407_gpio.h"
#include "at32f403a_407_misc.h"
#include <stdint.h>
#include "at32f403a_407_spi.h"
#include "gpio.h"
#include "log.h"

uint8_t get_spi_index(SPIPort *dev){
  if (dev->controller == SPI1) return 1;
#if defined(SPI2)
  else if (dev->controller == SPI2) return 2;
#endif
#if defined(SPI3)
  else if (dev->controller == SPI3) return 3;
#endif
#if defined(SPI4)
  else if (dev->controller == SPI4) return 4;
#endif
  else ASSERT(FALSE);
  return 0;
}

void spi_clock_enable(spi_type * dev){
  if (dev == SPI1){
    crm_periph_clock_enable(CRM_SPI1_PERIPH_CLOCK, TRUE);
  }
#if defined(SPI2)
  else if (dev == SPI2){
    crm_periph_clock_enable(CRM_SPI2_PERIPH_CLOCK, TRUE);
  }
#endif
#if defined(SPI3)
  else if (dev == SPI3){
    crm_periph_clock_enable(CRM_SPI3_PERIPH_CLOCK, TRUE);
  }
#endif
#if defined(SPI4)
  else if (dev == SPI4){
    crm_periph_clock_enable(CRM_SPI4_PERIPH_CLOCK, TRUE);
  }
#endif
  else {
    ASSERT(FALSE);
  }
}

void enable_spi_irq(spi_type * dev, uint32_t preempt_priority, uint32_t sub_priority){
  if (dev == SPI1){
    nvic_irq_enable(SPI1_IRQn, preempt_priority, sub_priority);
    spi_i2s_interrupt_enable(dev, SPI_I2S_RDBF_INT, TRUE);
  }
#if defined(SPI2)
  else if (dev == SPI2){
    nvic_irq_enable(SPI2_I2S2EXT_IRQn, preempt_priority, sub_priority);
    spi_i2s_interrupt_enable(dev, SPI_I2S_RDBF_INT, TRUE);
    //printf("spi2 irq enabled\n");
  }
#endif
#if defined(SPI3)
  else if (dev == SPI3){
    nvic_irq_enable(SPI3_I2S3EXT_IRQn, preempt_priority, sub_priority);
    spi_i2s_interrupt_enable(dev, SPI_I2S_RDBF_INT, TRUE);
  }
#endif
#if defined(SPI4)
  else if (dev == SPI4){
    nvic_irq_enable(SPI4_IRQn, preempt_priority, sub_priority);
    spi_i2s_interrupt_enable(dev, SPI_I2S_RDBF_INT, TRUE);
  }
#endif
  else {
    ASSERT(FALSE);
  }  
}

void init_spi(SPIPort *devport){
  ASSERT(devport);
  
  if (devport->init_type.master_slave_mode == SPI_MODE_SLAVE){ /* slave mode */
    init_gpio_input(&devport->clk, GPIO_PULL_UP, GPIO_DRIVE_STRENGTH_STRONGER);
    if (devport->cs.port != NULL) 
      init_gpio_input(&devport->cs, GPIO_PULL_UP, GPIO_DRIVE_STRENGTH_STRONGER);
    if (devport->mosi.port != NULL)
      init_gpio_input(&devport->mosi, GPIO_PULL_UP, GPIO_DRIVE_STRENGTH_STRONGER);
    if (devport->miso.port != NULL)
      init_gpio_mux(&devport->miso, GPIO_OUTPUT_PUSH_PULL, GPIO_PULL_UP, GPIO_DRIVE_STRENGTH_STRONGER);
  } else {			/* master mode */
    init_gpio_mux(&devport->clk, GPIO_OUTPUT_PUSH_PULL, GPIO_PULL_UP, GPIO_DRIVE_STRENGTH_STRONGER);
    if (devport->cs.port != NULL) {
      if (devport->init_type.cs_mode_selection == SPI_CS_HARDWARE_MODE)
	init_gpio_mux(&devport->cs, GPIO_OUTPUT_PUSH_PULL, GPIO_PULL_UP, GPIO_DRIVE_STRENGTH_STRONGER);
      else
	init_gpio_output(&devport->cs, GPIO_OUTPUT_PUSH_PULL, GPIO_DRIVE_STRENGTH_STRONGER);
    }
    if (devport->miso.port != NULL){
      init_gpio_input(&devport->miso, GPIO_PULL_UP, GPIO_DRIVE_STRENGTH_STRONGER);
    }
    if (devport->mosi.port != NULL){
      init_gpio_mux(&devport->mosi, GPIO_OUTPUT_PUSH_PULL, GPIO_PULL_UP, GPIO_DRIVE_STRENGTH_STRONGER);
    }
  }
  
  spi_clock_enable(devport->controller);
  
  spi_init(devport->controller, &devport->init_type);
  spi_enable(devport->controller, TRUE);
}
