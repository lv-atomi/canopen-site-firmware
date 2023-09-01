#include "at32f403a_407_board.h"
#include "at32f403a_407_clock.h"
#include "oled.h"
#include "timer.h"
#include <stdio.h>
#include <string.h>


OLEDPort oled = {
  .en = {GPIOC, GPIO_PINS_SOURCE13},
  .i2c = {
    .data = {GPIOA, GPIO_PINS_SOURCE12},
    .clk = {GPIOB, GPIO_PINS_SOURCE7},
    .address=0x78
  },
};

int main(){
  system_clock_config(); // 8M HSE
  at32_board_init();
  Timer_Init();
  uart_print_init(115200);	/* init debug uart */

  printf("start\n");
  init_oled(&oled);
  char buf[20];
  uint16_t buflen = 0;
  
  while (1) {
    snprintf(buf, 20, "tick:%ld", get_ticks());
    buflen = strlen(buf);
    printf("%s\n", buf);
    oled_showstring(&oled, 0, 0, buf, buflen, 8, 1);
    oled_refresh(&oled);
    delay_sec(1);
  }
}

