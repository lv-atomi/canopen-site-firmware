#include "board.h"
#include "timer.h"
#include "ad7705.h"
#include "log.h"
#include <stdint.h>
#include <stdlib.h>

AD7705Port ad7705 = {
  .clk={GPIOB, GPIO_PINS_SOURCE3, .gpio_remap=SWJTAG_GMUX_010}, /* release PB3, PB4 main function */
  .din={GPIOB, GPIO_PINS_SOURCE5, .gpio_remap=SPI3_GMUX_0010},	/* remap SPI3 ports to PB3,4,5 */
  .dout={GPIOB, GPIO_PINS_SOURCE4, .gpio_remap=SPI3_GMUX_0010},
  .nCS={GPIOA, GPIO_PINS_SOURCE8},
  .reset={GPIOA, GPIO_PINS_SOURCE3},
  .nDRDY={GPIOA, GPIO_PINS_SOURCE15}, /* FIXME, jump wire from PA.10 to PA.15 */
  .controller=SPI3,
};

int main(void) {
  system_clock_config(); // 8M HSE
  at32_board_init();
  Timer_Init();
  uart_print_init(115200);	/* init debug uart */

  log_printf("start\n");

  init_ad7705(&ad7705);
  ad7705_setup(&ad7705, AD7705_CH1);
  /* ad7705_setup(&ad7705, AD7705_CH2); */
  while (1) {
    uint16_t adc0_val = ad7705GetADCSync(&ad7705, AD7705_CH1);
    printf("7705 ch0_raw:%u  degree:%ld\n", adc0_val, ConvertToDegree(adc0_val));
    /* uint16_t adc1_val = ad7705GetADCSync(&ad7705, AD7705_CH2); */
    /* printf("7705 ch1_raw:%u  degree:%ld\n", adc1_val, ConvertToDegree(adc1_val)); */
    
    delay_ms(1000);
  }
}
