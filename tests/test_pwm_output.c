#include "at32f403a_407_board.h"
#include "at32f403a_407_clock.h"
#include "timer.h"
#include "pwm.h"

PWMPort port={
  .port={GPIOB, GPIO_PINS_6},	/* PB6 */
  .tmr=TMR4,
  .channel=TMR_SELECT_CHANNEL_1,
  .complementary=FALSE,
};

int main(void) {
  system_clock_config(); // 8M HSE
  at32_board_init();
  //Timer_Init();
  uart_print_init(115200);	/* init debug uart */

  printf("start\n");
  init_pwm_output(&port, 100, 50);
  
  while (1) {
    /* printf("tick:%ld\n", get_ticks()); */
    delay_sec(1);
    pwm_output_update_duty(&port, 30);
    delay_sec(1);
    pwm_output_update_duty(&port, 70);
    delay_sec(1);
    pwm_output_update_freq(&port, 500);
    delay_sec(1);
    pwm_output_update_freq(&port, 100);
  }
}
