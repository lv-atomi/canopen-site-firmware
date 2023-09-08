#include "at32f403a_407_board.h"
#include "at32f403a_407_clock.h"
#include "at32f403a_407_gpio.h"
#include "timer.h"
#include "motor.h"
#include "log.h"
#include <stdint.h>
#include <stdlib.h>

MotorUnified motor_brush = {
  .brush = {
    .disable = {GPIOB, GPIO_PINS_SOURCE14},
    .pwm_a = {
      .port = {GPIOA, GPIO_PINS_SOURCE10},
      .tmr = TMR1,
      .channel = TMR_SELECT_CHANNEL_3,
      .complementary = TRUE,
      .deadtime = 6,
    },
    .pwm_b = {
      .port = {GPIOB, GPIO_PINS_SOURCE1,
	       .gpio_remap=TMR1_MUX_01},
      .tmr = TMR1,
      .channel = TMR_SELECT_CHANNEL_3C,
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

  log_printf("start\n");
  /* init_gpio_output(&motor_brush.brush.pwm_a.port, */
  /* 		   GPIO_OUTPUT_PUSH_PULL, */
  /* 		   GPIO_DRIVE_STRENGTH_STRONGER); */
  /* init_gpio_output(&motor_brush.brush.pwm_b.port, */
  /* 		   GPIO_OUTPUT_PUSH_PULL, */
  /* 		   GPIO_DRIVE_STRENGTH_STRONGER); */

  init_motor_brush(&motor_brush);
  motor_set_speed(&motor_brush, 50);
  int8_t speed = 0;
  int8_t dir = 1;
  while (1) {
    /* gpio_set(&motor_brush.brush.pwm_a.port, dir); */
    /* gpio_set(&motor_brush.brush.pwm_b.port, !dir); */
    //printf("tick:%ld\n", get_ticks());
    //delay_sec(1);
    if (speed >=98) dir = -1;
    else if(speed <=-98) dir = 1;
    speed += dir;
    motor_set_speed(&motor_brush, speed);
    //delay_ms(abs(speed)*3);
    delay_ms(50);
    /* dir = !dir; */
  }
}
