#include "at32f403a_407_board.h"
#include "at32f403a_407_clock.h"
#include "timer.h"
#include "gpio.h"

IOPort port={GPIOB, GPIO_PINS_6};	/* PB6 */

int main(void) {
  system_clock_config(); // 8M HSE
  at32_board_init();
  //Timer_Init();
  uart_print_init(115200);	/* init debug uart */

  init_gpio_output(&port,
		   GPIO_OUTPUT_PUSH_PULL,
		   GPIO_MODE_OUTPUT,
		   GPIO_DRIVE_STRENGTH_STRONGER);
  
  while (1) {
    printf("tick:%ld\n", get_ticks());
    gpio_set(&port, 1);
    delay_sec(1);
    gpio_set(&port, 0);
    delay_sec(1);
  }
}
