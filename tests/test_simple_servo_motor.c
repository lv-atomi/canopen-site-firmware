#include "at32f403a_407_board.h"
#include "at32f403a_407_gpio.h"
#include "timer.h"
#include "log.h"
#include "simple_servo.h"
#include <stdint.h>
#include <stdlib.h>

SimpleServo motor = {
  .motor = {
    .brush = {
      .disable = {GPIOB, GPIO_PINS_SOURCE14},
      .pwm_a = {
	.port = {GPIOA, GPIO_PINS_SOURCE10},
	.tmr = TMR1,
	.channel = TMR_SELECT_CHANNEL_3,
	.complementary = TRUE,
	.deadtime = 6,
      },
      .pwm_b = {
	.port = {GPIOB, GPIO_PINS_SOURCE1,
		 .gpio_remap = TMR1_MUX_01},
	.tmr = TMR1,
	.channel = TMR_SELECT_CHANNEL_3C,
	.complementary = TRUE,
      }},
    .is_brushless = 0},
  /* .limit_a = {GPIOA, GPIO_PINS_SOURCE0}, */
  /* .limit_b = {GPIOA, GPIO_PINS_SOURCE1}, */
  .displace = {
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
  },
  .homing_direction = TOWARD_LIMIT_A,
};

int main(void) {
  system_clock_config(); // 8M HSE
  at32_board_init();
  Timer_Init();
  uart_print_init(115200);	/* init debug uart */

  log_printf("start\n");

  init_simple_servo(&motor, 99);
  log_printf("homing workload\n");
  delay_sec(1);
  
  log_printf("rewind...\n");
  ss_set_speed(&motor, 90, 1);
  log_printf("for 2 seconds\n");
  delay_sec(2);
  log_printf("done, waiting for 1 seconds\n");
  ss_set_stop(&motor);
  delay_sec(1);

  log_printf("start homing...\n");  
  ss_home(&motor, FALSE);
  while (ss_is_homing(&motor)) delay_ms(1);
  
  log_printf("homing finished, checking result...\n");    
  if (motor._homing_result == HR_SUCCESS){
    log_printf("homing success\n");
  } else {
    log_printf("homing failed: %s\n", HomingResultToString(motor._homing_result));
  }
  
  while (1) {
    ss_go_position_absolute(&motor, 20, 100);
    while (ss_is_moving(&motor)) delay_ms(1);
    delay_ms(1000);
    ss_go_position_absolute(&motor, 10, 100);
    while (ss_is_moving(&motor)) delay_ms(1);
    delay_ms(1000);
    /* dir = !dir; */
  }
}
