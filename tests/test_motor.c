#include "at32f403a_407_board.h"
#include "at32f403a_407_clock.h"
#include "at32f403a_407_gpio.h"
#include "timer.h"
#include "motor.h"
#include "log.h"
#include "oled.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

OLEDPort oled = {
    .en = {GPIOC, GPIO_PINS_SOURCE13},
    .i2c = {.data = {GPIOA, GPIO_PINS_SOURCE12},
            .clk = {GPIOB, GPIO_PINS_SOURCE7},
            .address = 0x78},
};

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
  init_oled(&oled);
  init_motor_brush(&motor_brush);
  motor_set_speed(&motor_brush, 50);
  
  char buf[20];
  uint16_t buflen = 0;
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
    snprintf(buf, 20, "spd:%d   ", speed);
    buflen = strlen(buf);
    oled_showstring(&oled, 0, 0, buf, buflen, 8, 1);
    oled_refresh(&oled);
    motor_set_speed(&motor_brush, speed);
    //delay_ms(abs(speed)*3);
    delay_ms(50);
    /* dir = !dir; */
  }
}
