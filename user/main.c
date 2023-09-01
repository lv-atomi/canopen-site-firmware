#include "at32f403a_407_board.h"
#include "at32f403a_407_clock.h"
#include "bmp.h"
/* #include "can.h" */
#include "can.h"
#include "flash.h"
#include "timer.h"
#include "ui.h"
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

  printf("System clock: %d\n", system_core_clock);
  printf("AHBDIV: %d apb1div:%d apb2div:%d\n",
	 CRM->cfg_bit.ahbdiv, CRM->cfg_bit.apb1div, CRM->cfg_bit.apb2div);

  init_ui();
  can_gpio_config();
  canopen_init();

  /* nvic_priority_group_config(NVIC_PRIORITY_GROUP_4); */

  printf("start\n");
  while (1) {
    if (ui_tick())
      continue;
    canopen_app_process();
    delay_ms(1);
  }
}
