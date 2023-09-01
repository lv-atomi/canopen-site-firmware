#include "at32f403a_407_board.h"
#include "at32f403a_407_clock.h"
#include "timer.h"
#include "ui.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

uint8_t stored_id = 1;
uint16_t stored_timeout = 0;

void my_store_id_cb(uint8_t id) {
  stored_id = id;  
}
uint8_t my_get_id_cb(void) {
  return stored_id;
}
void my_store_timeout_cb(uint16_t timeout) {
  stored_timeout = timeout;  
}
uint16_t my_get_timeout_cb(void) {
  return stored_timeout;  
}

/* get_id_cb * func_get_id_cb = NULL; */
/* store_id_cb * func_store_id_cb = NULL; */
/* store_timeout_cb * func_store_timeout_cb = NULL; */
/* get_timeout_cb * func_get_timeout_cb = NULL; */

int main(){
  system_clock_config(); // 8M HSE
  at32_board_init();
  Timer_Init();
  uart_print_init(115200);	/* init debug uart */

  printf("start\n");
  init_ui(my_store_id_cb,
	  my_get_id_cb,
	  my_store_timeout_cb,
	  my_get_timeout_cb);
  
  while (1) {
    if (ui_tick())
      continue;
    delay_ms(1);
  }
}
