#include "timer.h"
#include "gpiolink_crc8.h"
#include "log.h"

gpiolink link ={
  .port={GPIOB, GPIO_PINS_SOURCE1}
};

int main(void) {
  uint16_t i;
  system_clock_config(); // 8M HSE
  at32_board_init();
  Timer_Init();
  uart_print_init(115200);	/* init debug uart */
  log_printf("start\n");

  init_gpiolink(&link, TRUE);
  
  while (1) {
    for (i=0; i< 255; i++){
      gpiolink_master_send_byte(&link, 0x12);
      log_printf("sent: %u\n", i);
      delay_ms(10);
      /* ASSERT(link.state == GL_STATE_IDLE); */
    }
  }
}
