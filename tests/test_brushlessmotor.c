#include "at32f403a_407_board.h"
#include "at32f403a_407_clock.h"
#include "timer.h"
#include "motor.h"
#include "log.h"

MotorUnified motor_burshless = {
  .brushless = {
    .direction = {GPIOB, GPIO_PINS_SOURCE0},
    .speed_sense ={
      .port = {GPIOA, GPIO_PINS_SOURCE10},
      .tmr = TMR1,
      .channel = TMR_SELECT_CHANNEL_3,
      .complementary = FALSE,
    },
    .speed_set ={
      .port = {GPIOB, GPIO_PINS_SOURCE1},
      .tmr = TMR3,
      .channel = TMR_SELECT_CHANNEL_4,
      .complementary = FALSE,
    }
  },
  .is_brushless = 1
};

int main(void) {
  system_clock_config(); // 8M HSE
  at32_board_init();
  Timer_Init();
  uart_print_init(115200);	/* init debug uart */

  log_printf("start\n");
  init_motor_brushless(&motor_burshless);
  
  while (1) {
    log_printf("tick:%ld\n", get_ticks());
    delay_sec(1);
  }
}
