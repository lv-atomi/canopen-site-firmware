#include "at32f403a_407_board.h"
#include "at32f403a_407_clock.h"
#include "timer.h"

int main(void) {
  system_clock_config(); // 8M HSE
  at32_board_init();
  Timer_Init();
  uart_print_init(115200);	/* init debug uart */

  printf("start\n");
  while (1) {
    printf("tick:%ld\n", get_ticks());
    delay_sec(1);
  }
}
