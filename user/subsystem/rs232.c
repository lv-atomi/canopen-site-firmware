#include "rs232.h"
#include "gpio.h"
#include "log.h"
#include <stdint.h>

void init_usart_clock(usart_type * dev){
#ifdef USART1
  if (dev == USART1){
    crm_periph_clock_enable(CRM_USART1_PERIPH_CLOCK, TRUE);
  } else
#endif
#ifdef USART2
  if (dev == USART2){
    crm_periph_clock_enable(CRM_USART2_PERIPH_CLOCK, TRUE);    
  } else
#endif
#ifdef USART3
  if (dev == USART3){
    crm_periph_clock_enable(CRM_USART3_PERIPH_CLOCK, TRUE);
  } else
#endif
#ifdef UART4
  if (dev == UART4){
    crm_periph_clock_enable(CRM_UART4_PERIPH_CLOCK, TRUE);
  } else
#endif
#ifdef UART5
  if (dev == UART5){
    crm_periph_clock_enable(CRM_UART5_PERIPH_CLOCK, TRUE);
  } else
#endif
#ifdef USART6
  if (dev == USART6){
    crm_periph_clock_enable(CRM_USART6_PERIPH_CLOCK, TRUE);
  } else
#endif
#ifdef UART7
  if (dev == UART7){
    crm_periph_clock_enable(CRM_UART7_PERIPH_CLOCK, TRUE);
  } else
#endif
#ifdef UART8
  if (dev == UART8){
    crm_periph_clock_enable(CRM_UART8_PERIPH_CLOCK, TRUE);
  } else
#endif
    {
    }
}

void init_rs232(RS232Port *devport) {
  ASSERT(devport);

  if (devport->TX.port != NULL)
    init_gpio_mux(&devport->TX, GPIO_OUTPUT_PUSH_PULL,
		  GPIO_PULL_NONE, GPIO_DRIVE_STRENGTH_STRONGER);
  if (devport->RX.port != NULL)
    init_gpio_input(&devport->RX, GPIO_PULL_UP, GPIO_DRIVE_STRENGTH_STRONGER);

  init_usart_clock(devport->controller);
  usart_init(devport->controller, devport->baud_rate,
	     devport->data_bit, devport->stop_bit);
  
  usart_flag_clear(devport->controller, USART_RDBF_FLAG);
  /* usart_interrupt_enable(USART2, USART_RDBF_INT, TRUE); */

  if (devport->RX.port != NULL)
    usart_receiver_enable(devport->controller, TRUE);
  if (devport->TX.port != NULL)
    usart_transmitter_enable(devport->controller, TRUE);

  usart_enable(devport->controller, TRUE);
}

void rs232_transmit(RS232Port *devport, uint8_t *buf, uint16_t size) {
  ASSERT(devport);

  while(size--){
    while(usart_flag_get(devport->controller, USART_TDBE_FLAG) == RESET);
    usart_data_transmit(devport->controller, *buf++);
  }
  while(usart_flag_get(devport->controller, USART_TDC_FLAG) == RESET);
}

bool_t rs232_receive(RS232Port *devport, uint16_t *buf, uint16_t size, uint32_t timeout) {
  ASSERT(devport);

  uint16_t idx = 0;
  while(idx < size){
    while(usart_flag_get(devport->controller, USART_RDBF_FLAG) == RESET);
    buf[idx++] = usart_data_receive(devport->controller);
  }
  return 0;
}

