#include "can.h"
#include "flash.h"

#include "CO_app_STM32.h"
#include "timer.h"
#include <stdint.h>
#include <sys/types.h>

/* uint16_t testkey1 = 0, testkey2 = 0; */
//extern uint8_t need_reconfigure_can;

SysParm_TypeDef gSysParm;
SysEprom_TypeDef gSysEpromData;
uint16_t buffer_write[TEST_BUFEER_SIZE]; //[0]:ID; [1]:Voltage
uint16_t buffer_read[TEST_BUFEER_SIZE];  //[0]:ID; [1]:Voltage
can_type hcan;

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

/* void can_transmit_data(void) { */
/*   uint8_t transmit_mailbox; */
/*   can_tx_message_type tx_message_struct; */
/*   tx_message_struct.standard_id = gSysParm.basePara.CanDataId; */
/*   tx_message_struct.extended_id = 0; */
/*   tx_message_struct.id_type = CAN_ID_STANDARD; */
/*   tx_message_struct.frame_type = CAN_TFT_DATA; */
/*   tx_message_struct.dlc = 8; */
/*   tx_message_struct.data[0] = testkey1; // gSysParm.canRxBuf[0]; */
/*   tx_message_struct.data[1] = testkey2; // gSysParm.canRxBuf[1]; */
/*   tx_message_struct.data[2] = gSysParm.canRxBuf[2]; */
/*   tx_message_struct.data[3] = gSysParm.canRxBuf[3]; */
/*   tx_message_struct.data[4] = gSysParm.canRxBuf[4]; */
/*   tx_message_struct.data[5] = gSysParm.canRxBuf[5]; */
/*   tx_message_struct.data[6] = gSysParm.canRxBuf[6]; */
/*   tx_message_struct.data[7] = gSysParm.canRxBuf[7]; */
/*   transmit_mailbox = can_message_transmit(CAN1, &tx_message_struct); */
/*   //  while(can_transmit_status_get(CAN1, */
/*   //  (can_tx_mailbox_num_type)transmit_mailbox) != CAN_TX_STATUS_SUCCESSFUL); */
/* } */


void canopen_init(uint8_t station_id){
  static CANopenNodeSTM32 canOpenNodeSTM32;
  canOpenNodeSTM32.CANHandle = CAN1;
  canOpenNodeSTM32.HWInitFunction = can_configuration;
  canOpenNodeSTM32.desiredNodeID = station_id;
  canOpenNodeSTM32.baudrate = 500;
  canopen_app_init(&canOpenNodeSTM32);
  log_printf("init, station id:%d baudrate:%d\n",
	     canOpenNodeSTM32.desiredNodeID,
	     canOpenNodeSTM32.baudrate);
  timer_add_tick(canopen_app_interrupt);
}

/* uint8_t can_read_para(void) { */
/*   uint8_t checkSum = 0, i = 0; */

/*   for (i = 0; i < 7; i++) { */
/*     checkSum += gSysParm.canRxBuf[i]; */
/*   } */
/*   if (checkSum != */
/*       gSysParm.canRxBuf[7]) // check checksum is right or not, 1: wrong;0:right */
/*   { */
/*     return 1; */
/*   } */

/*   // analyze CMD command */
/*   gSysParm.basePara.RWCmd = (gSysParm.canRxBuf[3] & 0x03); */

/*   // Write or Read command */
/*   if (gSysParm.basePara.RWCmd == BASE_READ) // Read command */
/*   { */

/*   } else if (gSysParm.basePara.RWCmd == BASE_WRITE) // Write command */
/*   { */

/*   } else { */
/*     return 1; // illegal commond */
/*   } */
/*   return 0; */
/* } */

/* void CAN_DataInit() { */
/*   uint16_t i; */
/*   gSysParm.basePara.CanDataId = SLAVEBOARD_ID; */
/*   keySta.Dismode = 0; // Display normal */
/*   keySta.KeyCnt10s = 0; */

/*   for (i = 0; i < TEST_BUFEER_SIZE; i++) // data buffer clear */
/*   { */
/*     buffer_read[i] = 0x00; */
/*     buffer_write[i] = 0x00; */
/*   } */
/* } */
