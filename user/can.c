#include "can.h"
#include "flash.h"

#include "CO_app_STM32.h"
#include "timer.h"
#include <stdint.h>
#include <sys/types.h>

void can_gpio_config(void) {
  gpio_init_type gpio_init_struct;

  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_IOMUX_PERIPH_CLOCK, TRUE);
  gpio_pin_remap_config(CAN1_GMUX_0010, TRUE);

  gpio_default_para_init(&gpio_init_struct);

  /* can tx pin */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_pins = GPIO_PINS_9;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init(GPIOB, &gpio_init_struct);

  /* can rx pin */
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_pins = GPIO_PINS_8;
  gpio_init_struct.gpio_pull = GPIO_PULL_UP;
  gpio_init(GPIOB, &gpio_init_struct);
}

void can_configuration(void) {
  can_base_type can_base_struct;
  can_baudrate_type can_baudrate_struct;
  can_filter_init_type can_filter_init_struct;

  //need_reconfigure_can = 0;
  crm_periph_clock_enable(CRM_CAN1_PERIPH_CLOCK, TRUE);
  /* can base init */
  can_default_para_init(&can_base_struct);
  can_base_struct.mode_selection = CAN_MODE_COMMUNICATE;
  can_base_struct.ttc_enable = FALSE;
  can_base_struct.aebo_enable = TRUE;
  can_base_struct.aed_enable = TRUE;
  can_base_struct.prsf_enable = FALSE;
  can_base_struct.mdrsel_selection = CAN_DISCARDING_FIRST_RECEIVED;
  can_base_struct.mmssr_selection = CAN_SENDING_BY_ID;
  can_base_init(CAN1, &can_base_struct);

  /* can baudrate, set baudrate = pclk/(baudrate_div *(1 + bts1_size +
   * bts2_size)) */

#ifdef BAUD_1M // sample point = 75%
  can_baudrate_struct.baudrate_div = 10; /* AHB clock = 120M BRDIV=12*/
  can_baudrate_struct.rsaw_size = CAN_RSAW_1TQ;
  can_baudrate_struct.bts1_size = CAN_BTS1_8TQ;
  can_baudrate_struct.bts2_size = CAN_BTS2_3TQ;
  can_baudrate_set(CAN1, &can_baudrate_struct);
#endif

#ifdef BAUD_500K // sample point = 80%
  /* can_baudrate_struct.baudrate_div = 12; /\* AHB clock = 120M BRDIV=10*\/ */
  /* can_baudrate_struct.rsaw_size = CAN_RSAW_2TQ; */
  /* can_baudrate_struct.bts1_size = CAN_BTS1_15TQ; */
  /* can_baudrate_struct.bts2_size = CAN_BTS2_2TQ; */

  can_baudrate_struct.baudrate_div = 15; /* AHB clock = 120M BRDIV=8*/
  can_baudrate_struct.rsaw_size = CAN_RSAW_2TQ;
  can_baudrate_struct.bts1_size = CAN_BTS1_12TQ;
  can_baudrate_struct.bts2_size = CAN_BTS2_3TQ;
  can_baudrate_set(CAN1, &can_baudrate_struct);
#endif

  /* can filter init */
  can_filter_init_struct.filter_activate_enable = TRUE;
  can_filter_init_struct.filter_mode = CAN_FILTER_MODE_ID_MASK;
  can_filter_init_struct.filter_fifo = CAN_FILTER_FIFO0;
  can_filter_init_struct.filter_number = 0;
  can_filter_init_struct.filter_bit = CAN_FILTER_32BIT;
  can_filter_init_struct.filter_id_high = 0;
  can_filter_init_struct.filter_id_low = 0;
  can_filter_init_struct.filter_mask_high = 0;
  can_filter_init_struct.filter_mask_low = 0;
  can_filter_init(CAN1, &can_filter_init_struct);

  /* can interrupt config */
  nvic_irq_enable(CAN1_SE_IRQn, 0x00, 0x00);
  nvic_irq_enable(USBFS_L_CAN1_RX0_IRQn, 0x00, 0x00);
  nvic_irq_enable(USBFS_H_CAN1_TX_IRQn, 0x00, 0x00);
  can_interrupt_enable(CAN1, CAN_TCIEN_INT, TRUE);
  can_interrupt_enable(CAN1, CAN_RF0MIEN_INT, TRUE);

  /* error interrupt enable */
  can_interrupt_enable(CAN1, CAN_ETRIEN_INT, TRUE);
  can_interrupt_enable(CAN1, CAN_EOIEN_INT, TRUE);
}

void canopen_init(){
  static CANopenNodeSTM32 canOpenNodeSTM32;
  canOpenNodeSTM32.CANHandle = CAN1;
  canOpenNodeSTM32.HWInitFunction = can_configuration;
  canOpenNodeSTM32.desiredNodeID = 1;
  canOpenNodeSTM32.baudrate = 500;
  canopen_app_init(&canOpenNodeSTM32);
  
  log_printf("init, desired station id:%d baudrate:%d\n",
	     canOpenNodeSTM32.desiredNodeID,
	     canOpenNodeSTM32.baudrate);
  timer_add_tick(canopen_app_interrupt, 0);
}

