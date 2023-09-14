#include "board.h"
#include "motor_485.h"
#include "timer.h"
#include "log.h"
#include <stdint.h>
#include <stdlib.h>

Motor485 motor_485 = {
  .port = {
    .DE_nRE={GPIOB, GPIO_PINS_SOURCE12},
    .port = {
      .TX={GPIOB, GPIO_PINS_SOURCE10},
      .RX={GPIOB, GPIO_PINS_SOURCE11},
      .controller=USART3,
      .baud_rate=38400,
      .data_bit=USART_DATA_8BITS,
      .stop_bit=USART_STOP_1_BIT
    }
  },
  .addr=1,
  .limit_left={GPIOA, GPIO_PINS_SOURCE0},
  .limit_right={GPIOA, GPIO_PINS_SOURCE1},
};

int main(void) {
  system_clock_config(); // 8M HSE
  at32_board_init();
  Timer_Init();
  uart_print_init(115200);	/* init debug uart */

  log_printf("start\n");

  init_motor_485(&motor_485);
  while (1) {
    /* gpio_set(&motor_brush.brush.pwm_a.port, dir); */
    /* gpio_set(&motor_brush.brush.pwm_b.port, !dir); */
    //printf("tick:%ld\n", get_ticks());
    //delay_sec(1);
    log_printf("move to +200\n");
    motor_go_pos(&motor_485, 2000, 50, Acc2);
    delay_sec(1);
    log_printf("move to -200\n");
    motor_go_pos(&motor_485, -2000, 100, Acc2);
    delay_sec(1);
  }
}
