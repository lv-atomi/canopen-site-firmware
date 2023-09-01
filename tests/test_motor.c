#include "at32f403a_407_board.h"
#include "at32f403a_407_clock.h"
#include "timer.h"
#include "motor.h"

MotorUnified motor_brush = {
  .brush = {
    .disable = {GPIOB, GPIO_PINS_SOURCE14},
    .pwm_a = {
      .port = {GPIOA, GPIO_PINS_SOURCE9},
      .tmr = TMR1,
      .channel = TMR_SELECT_CHANNEL_1,
      .complementary = TRUE,
    },
    .pwm_b = {
      .port = {GPIOB, GPIO_PINS_SOURCE0},
      .tmr = TMR1,
      .channel = TMR_SELECT_CHANNEL_1C,
      .complementary = TRUE,
    }
  },
  .is_brushless = 0
};

int main(void) {
  system_clock_config(); // 8M HSE
  at32_board_init();
  Timer_Init();
  uart_print_init(115200);	/* init debug uart */

  printf("start\n");
  init_motor_brush(&motor_brush);
  
  while (1) {
    printf("tick:%ld\n", get_ticks());
    delay_sec(1);
  }
}
