#include "at32f403a_407_board.h"
#include "at32f403a_407_clock.h"
#include "timer.h"
#include "ui.h"
#include <stdio.h>
#include <string.h>

int main(){
  system_clock_config(); // 8M HSE
  at32_board_init();
  Timer_Init();
  uart_print_init(115200);	/* init debug uart */

  printf("start\n");
  init_ui();
  
  while (1) {
    if (ui_tick())
      continue;
    delay_ms(1);
  }
}
