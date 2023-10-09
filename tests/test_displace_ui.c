/* #include "can.h" */
#include "cap_displacement.h"
#include "log.h"
#include "timer.h"
#include "oled.h"
#include <stdint.h>
#include <string.h>

OLEDPort oled = {
  .en = {GPIOC, GPIO_PINS_SOURCE13},
  .i2c = {
    .data = {GPIOA, GPIO_PINS_SOURCE12},
    .clk = {GPIOB, GPIO_PINS_SOURCE7},
    .address=0x78
  },
};

extern unsigned int system_core_clock; /*!< system clock frequency (core clock) */

CapacitorDisplacementMeasurePort displacement_test = {
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

  log_printf("System clock: %d\n", system_core_clock);
  log_printf("AHBDIV: %d apb1div:%d apb2div:%d\n",
	 CRM->cfg_bit.ahbdiv, CRM->cfg_bit.apb1div, CRM->cfg_bit.apb2div);
  
  log_printf("start\n");
  init_oled(&oled);
  init_capacitor_displacement_measurement(&displacement_test);
  
  char buf[20];
  uint16_t buflen = 0;
  //uint32_t last_delay = 0;
  while (1) {
    uint32_t delay = 0;
    int32_t pos = read_displacement(&displacement_test, &delay, 1);
    //if (delay < last_delay)
    {
      log_printf("displace: %ld, delay:%ld\n", pos, delay);
      snprintf(buf, 20, "%ld   ", pos);
      buflen = strlen(buf);
      oled_showstring(&oled, 0, 0, buf, buflen, 8, 1);
      
      snprintf(buf, 20, "%ld   ", delay);
      buflen = strlen(buf);
      oled_showstring(&oled, 0, 10, buf, buflen, 8, 1);

      snprintf(buf, 20, "%X %X %X  ",
	       displacement_test.buf[0],
	       displacement_test.buf[1],
	       displacement_test.buf[2]);
      buflen = strlen(buf);
      oled_showstring(&oled, 0, 20, buf, buflen, 8, 1);
      
      oled_refresh(&oled);
    }
    //last_delay = delay;
    delay_ms(50);
  }
}
