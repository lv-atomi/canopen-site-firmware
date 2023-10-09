#include "rs232.h"
#include "gpio.h"
#include "log.h"
#include <stdint.h>
#include "timer.h"


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

  log_printf("Initializing RS232 for controller: %p\n", devport->controller);

  if (devport->TX.port != NULL) {
    log_printf("Initializing TX GPIO for RS232...\n");
    init_gpio_mux(&devport->TX, GPIO_OUTPUT_PUSH_PULL,
		  GPIO_PULL_NONE, GPIO_DRIVE_STRENGTH_STRONGER);
  } else {
    log_printf("TX GPIO for RS232 is not defined.\n");
  }

  if (devport->RX.port != NULL) {
    log_printf("Initializing RX GPIO for RS232...\n");
    init_gpio_input(&devport->RX, GPIO_PULL_UP, GPIO_DRIVE_STRENGTH_STRONGER);
  } else {
    log_printf("RX GPIO for RS232 is not defined.\n");
  }

  log_printf("Initializing USART clock...\n");
  init_usart_clock(devport->controller);

  log_printf("Setting up USART with baud rate: %ld, data bit: %d, stop bit: %d\n", 
              devport->baud_rate, devport->data_bit, devport->stop_bit);
  usart_init(devport->controller, devport->baud_rate,
	     devport->data_bit, devport->stop_bit);
  
  log_printf("Clearing USART RDBF flag...\n");
  usart_flag_clear(devport->controller, USART_RDBF_FLAG);

  if (devport->RX.port != NULL) {
    log_printf("Enabling USART receiver...\n");
    usart_receiver_enable(devport->controller, TRUE);
  }

  if (devport->TX.port != NULL) {
    log_printf("Enabling USART transmitter...\n");
    usart_transmitter_enable(devport->controller, TRUE);
  }

  log_printf("Enabling USART controller...\n");
  usart_enable(devport->controller, TRUE);

  log_printf("RS232 initialization completed for controller: %p\n", devport->controller);
}

void rs232_flush(RS232Port *devport) {
  ASSERT(devport);

  usart_enable(devport->controller, FALSE);
  usart_enable(devport->controller, TRUE);
}

/* void init_rs232(RS232Port *devport) { */
/*   ASSERT(devport); */

/*   if (devport->TX.port != NULL) */
/*     init_gpio_mux(&devport->TX, GPIO_OUTPUT_PUSH_PULL, */
/* 		  GPIO_PULL_NONE, GPIO_DRIVE_STRENGTH_STRONGER); */
/*   if (devport->RX.port != NULL) */
/*     init_gpio_input(&devport->RX, GPIO_PULL_UP, GPIO_DRIVE_STRENGTH_STRONGER); */

/*   init_usart_clock(devport->controller); */
/*   usart_init(devport->controller, devport->baud_rate, */
/* 	     devport->data_bit, devport->stop_bit); */
  
/*   usart_flag_clear(devport->controller, USART_RDBF_FLAG); */
/*   /\* usart_interrupt_enable(USART2, USART_RDBF_INT, TRUE); *\/ */

/*   if (devport->RX.port != NULL) */
/*     usart_receiver_enable(devport->controller, TRUE); */
/*   if (devport->TX.port != NULL) */
/*     usart_transmitter_enable(devport->controller, TRUE); */

/*   usart_enable(devport->controller, TRUE); */
/* } */

void rs232_transmit(RS232Port *devport, uint8_t *buf, uint16_t size) {
  ASSERT(devport);

  while(size--){
    while(usart_flag_get(devport->controller, USART_TDBE_FLAG) == RESET);
    usart_data_transmit(devport->controller, *buf++);
  }
  while(usart_flag_get(devport->controller, USART_TDC_FLAG) == RESET);
}

bool_t rs232_receive(RS232Port *devport, uint8_t *buf, uint16_t size, uint32_t timeout) {
  ASSERT(devport);

  uint32_t elapsed_time = 0;
  uint32_t start_ticks = get_ticks(); // 获取开始时间
  uint16_t idx = 0;

  while (idx < size) {
    // 如果设置了超时，并且已经超过了超时时间，则返回失败
    if (timeout != 0 && elapsed_time >= timeout) {
      /* log_printf("Timeout occurred in rs232_receive after %ld ms.\n", */
      /*            timeout); // Optional, for debugging */
      return 0;            // Timeout occurred
    }

    // 如果数据可用，则读取它
    if (usart_flag_get(devport->controller, USART_RDBF_FLAG) != RESET) {
      buf[idx++] = usart_data_receive(devport->controller);
    }

    // 使用你提供的函数来更新经过的时间
    elapsed_time += ticks_diff(&start_ticks);
  }

  return 1; // Success
}

uint16_t rs232_receive_variable(RS232Port *devport, uint8_t *buf, uint16_t *max_size, uint16_t timeout) {
  ASSERT(devport);
  ASSERT(buf);
  ASSERT(max_size);

  uint32_t elapsed_time = 0;
  uint32_t start_ticks = get_ticks(); // 获取开始时间
  uint32_t temp_ticks = start_ticks;  // 临时变量用于ticks_diff
  uint16_t idx = 0;

  /* log_printf("Starting rs232_receive_variable. Max size: %d\n", *max_size); */

  while (idx < *max_size) {
    // 如果数据可用，则读取它
    if (usart_flag_get(devport->controller, USART_RDBF_FLAG) != RESET) {
      buf[idx++] = usart_data_receive(devport->controller);
      start_ticks = get_ticks(); // 重置开始时间，因为我们刚刚收到了数据
      //      log_printf("Received byte. Total bytes received: %d\n", idx);
    }
    temp_ticks = start_ticks;  // 同样重置临时变量
    // 使用你提供的函数来更新经过的时间
    elapsed_time = ticks_diff(&temp_ticks);  // 使用临时变量

    // 如果在数据包之间有小的时间间隔，则退出
    /* log_printf("elapsed_time:%ld\n", elapsed_time); */
    if (elapsed_time >= timeout) {
      /* log_printf("Timeout reached. Exiting after %ldms.\n", elapsed_time); */
      break;
    }
  }

  *max_size = idx; // 更新max_size为实际读取的字节数

  /* log_printf("Exiting rs232_receive_variable. Bytes read: %d\n", idx); */
  
  return idx; // 返回接收的数据长度
}
