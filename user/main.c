#include "at32f403a_407_board.h"
#include "at32f403a_407_clock.h"
#include "bmp.h"
/* #include "can.h" */
#include "can.h"
#include "flash.h"
#include "key.h"
#include "oled.h"
#include "timer.h"

#include "CO_app_STM32.h"
#include <stdint.h>

//extern SysParm_TypeDef gSysParm;
/* extern KeyPressMode keySta; */
extern unsigned int system_core_clock; /*!< system clock frequency (core clock) */
__IO uint8_t need_reconfigure_can;    

//uint16_t VolShowTest = DEFAULTVOL;
int main(void) {
  system_clock_config(); // 8M HSE
  at32_board_init();
  Timer_Init();

  uart_print_init(115200);	/* init debug uart */

  /* printf("System clock: %d\n", system_core_clock); */
  /* printf("AHBDIV: %d apb1div:%d apb2div:%d\n", */
  /* 	 CRM->cfg_bit.ahbdiv, CRM->cfg_bit.apb1div, CRM->cfg_bit.apb2div); */

  /* can_gpio_config(); */
  /* canopen_init(); */

  /* Timer1_Init(); */
  /* OLED_Init(); */
  /* GPIO_Init(); */
  /* nvic_priority_group_config(NVIC_PRIORITY_GROUP_4); */


  /* can_gpio_config(); */
  /* can_configuration(); */
  /* CAN_DataInit(); // Data Init */

  /* read data from flash */
  //flash_read(TEST_FLASH_ADDRESS_START, buffer_read, TEST_BUFEER_SIZE);
  /* gSysParm.basePara.CanDataId = buffer_read[0]; */
  /* if ((gSysParm.basePara.CanDataId >= 0x7ff) || */
  /*     (gSysParm.basePara.CanDataId == 0)) */
  /*   gSysParm.basePara.CanDataId = SLAVEBOARD_ID; */
  /* VolShowTest = buffer_read[1]; */
  /* if ((VolShowTest > 350) || (VolShowTest == 0)) */
  /*   VolShowTest = DEFAULTVOL; */
  
  printf("start\n");
  while (1) {
    printf("tick:%ld\n", get_ticks());
    delay_sec(1);
    
    /* if (need_reconfigure_can){ */
    /*   printf("reconfigure can\n"); */
    /*   can_configuration(); */
    /*   need_reconfigure_can = 0; */
    /* } */
    /* OLED_ShowString(0, 0, debug, 16, 1); */
    /* OLED_Refresh(); */
    /* canopen_app_process(); */
    //DisplayPra(keySta.Dismode); // OLED Display
    /* if (TimerCount_1ms == 1)    // Every 100ms send can frame */
    /* { */
    /*   TimerCount_1ms = 0; */
    /*   can_transmit_data(); // Transmit can date */
    /* } */

    /* if (gSysParm.canRxCmd == CAN_RXD_DATA) // if Received Read command */
    /* { */
    /*   gSysParm.canRxCmd = CAN_RXD_NULL; */
    /*   if (can_read_para() == 0) // check sum is correct */
    /*   { */
    /*     if (gSysParm.basePara.RWCmd == BASE_WRITE) { */
    /*     } */
    /*     gSysParm.canTxCmd = CAN_TXD_DATA; // Send data flag */
    /*   } */
    /* } */

    /* if (gSysParm.canTxCmd == CAN_TXD_DATA) // send can parameter */
    /* { */
    /*   gSysParm.canTxCmd = CAN_TXD_NULL; */
    /* } */
  }
}
