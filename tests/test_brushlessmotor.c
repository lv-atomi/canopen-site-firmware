#include "at32f403a_407_board.h"
#include "at32f403a_407_clock.h"
#include "timer.h"
#include "motor.h"

MotorUnified motor_burshless = {
  .brushless = {
    .direction = {GPIOB, GPIO_PINS_SOURCE1},
    .speed_sense ={
      .port = {GPIOA, GPIO_PINS_SOURCE9},
      .tmr = TMR1,
      .channel = TMR_SELECT_CHANNEL_2,
      .complementary = FALSE,
    },
    .speed_set ={
      .port = {GPIOB, GPIO_PINS_SOURCE0},
      .tmr = TMR3,
      .channel = TMR_SELECT_CHANNEL_3,
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

  printf("start\n");
  init_motor_brushless(&motor_burshless);
  
  while (1) {
    printf("tick:%ld\n", get_ticks());
    delay_sec(1);
  }
}
