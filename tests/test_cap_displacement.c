#include "at32f403a_407_board.h"
#include "at32f403a_407_clock.h"
#include "at32f403a_407_spi.h"
#include "bmp.h"
/* #include "can.h" */
#include "cap_displacement.h"
#include "flash.h"
#include "log.h"
#include "timer.h"
#include "ui.h"
#include <stdint.h>
//#include "app_common.h"

extern unsigned int system_core_clock; /*!< system clock frequency (core clock) */

CapacitorDisplacementMeasurePort displacement = {
  .spi={
    .miso = {GPIOB, GPIO_PINS_SOURCE15},
    .clk = {GPIOB, GPIO_PINS_SOURCE13},
    .controller = SPI2,
    .init_type = {
      .transmission_mode = SPI_TRANSMIT_SIMPLEX_RX,
      .master_slave_mode = SPI_MODE_SLAVE,
      .mclk_freq_division = SPI_MCLK_DIV_32,
      .first_bit_transmission = SPI_FIRST_BIT_LSB,
      .frame_bit_num = SPI_FRAME_8BIT,
      .clock_polarity = SPI_CLOCK_POLARITY_HIGH,
      .clock_phase = SPI_CLOCK_PHASE_2EDGE,
      .cs_mode_selection = SPI_CS_SOFTWARE_MODE,
    }
  }
};

//uint16_t VolShowTest = DEFAULTVOL;
int main(void) {
  system_clock_config(); // 8M HSE
  at32_board_init();
  Timer_Init();

  uart_print_init(115200);	/* init debug uart */

  printf("System clock: %d\n", system_core_clock);
  printf("AHBDIV: %d apb1div:%d apb2div:%d\n",
	 CRM->cfg_bit.ahbdiv, CRM->cfg_bit.apb1div, CRM->cfg_bit.apb2div);
  
  log_printf("start\n");
  init_capacitor_displacement_measurement(&displacement);
  
  while (1) {
    uint32_t delay = 0;
    int32_t pos = read_displacement(&displacement, &delay, 1);
    printf("displace: %ld, delay:%ld\n", pos, delay);
    delay_ms(100);
  }
}
