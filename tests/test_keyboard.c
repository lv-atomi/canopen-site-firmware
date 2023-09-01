#include "at32f403a_407_board.h"
#include "at32f403a_407_clock.h"
#include "keyboard.h"
#include "timer.h"

KeyboardPort enter = {
  .port={GPIOC, GPIO_PINS_SOURCE14},
  .capture=KEY_ACTIVE_LOW
};

KeyboardPort select = {
  .port={GPIOC, GPIO_PINS_SOURCE15},
  .capture=KEY_ACTIVE_LOW
};

int main(){
  system_clock_config(); // 8M HSE
  at32_board_init();
  Timer_Init();
  uart_print_init(115200);	/* init debug uart */

  printf("start\n");
  init_keyboard(&enter);
  init_keyboard(&select);
  
  while (1) {
    KeyboardPort * port;
    enum KEYSTATUS status;
    if(keyboard_get_event(&port, &status)){
      printf("Key:%s  evt:%s\n",
	     port == &enter ? "enter" : "select",
	     status == KEY_PRESS ? "PRESS" : \
	     status == KEY_RELEASE ? "RELEASE" :\
	     status == KEY_CLICKED ? "CLICK" :\
	     status == KEY_LONGPRESS ? "LONGPRESS" : "DUMMY"
	     );
      continue;
    }
    delay_ms(1);
  }
}
