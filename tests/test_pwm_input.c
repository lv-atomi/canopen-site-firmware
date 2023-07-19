#include "at32f403a_407_board.h"
#include "at32f403a_407_clock.h"
#include "timer.h"
#include "pwm.h"

PWMPort port_ch1={
  .port={GPIOB, GPIO_PINS_6},	/* PB6 */
  .tmr=TMR4,
  .channel=TMR_SELECT_CHANNEL_1,
  .complementary=FALSE,
};

PWMPort port_ch2={
  .port={GPIOB, GPIO_PINS_7},	/* PB7 */
  .tmr=TMR4,
  .channel=TMR_SELECT_CHANNEL_2,
  .complementary=FALSE,
};

PWMPort port_ch3={
  .port={GPIOB, GPIO_PINS_8},	/* PB8 */
  .tmr=TMR4,
  .channel=TMR_SELECT_CHANNEL_3,
  .complementary=FALSE,
};

int main(void) {
  system_clock_config(); // 8M HSE
  at32_board_init();
  //Timer_Init();
  uart_print_init(115200);	/* init debug uart */

  printf("start\n");
  init_pwm_input(&port_ch2);

  uint32_t freq=0;
  uint8_t duty=0;
  while (1) {
    /* printf("tick:%ld\n", get_ticks()); */
    delay_sec(1);
    pwm_input_freq_duty_sense(&port_ch2, &freq, &duty);
    printf("freq:%ld, duty:%u\n", freq, duty);
    /* delay_sec(1); */
    /* pwm_input_freq_sense(&port_ch2); */
    /* delay_sec(1); */
  }
}
