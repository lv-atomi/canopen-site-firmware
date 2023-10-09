#include "timer.h"
#include "motor.h"
#include "log.h"
#include "oled.h"
#include "cap_displacement.h"
#include <stdint.h>
#include <stdlib.h>

MotorUnified motor_brush = {
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
	       .gpio_remap=TMR1_MUX_01},
      .tmr = TMR1,
      .channel = TMR_SELECT_CHANNEL_3C,
      .complementary = TRUE,
    }
  },
  .is_brushless = 0
};

OLEDPort oled = {
    .en = {GPIOC, GPIO_PINS_SOURCE13},
    .i2c = {.data = {GPIOA, GPIO_PINS_SOURCE12},
            .clk = {GPIOB, GPIO_PINS_SOURCE7},
            .address = 0x78},
};

CapacitorDisplacementMeasurePort displacement_test = {
    .spi = {.miso = {GPIOB, GPIO_PINS_SOURCE15},
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
            }}};

int main(void) {
  system_clock_config(); // 8M HSE
  at32_board_init();
  Timer_Init();
  uart_print_init(115200);	/* init debug uart */

  char buf[20];
  uint16_t buflen = 0;
  
  log_printf("start\n");

  init_oled(&oled);
  init_capacitor_displacement_measurement(&displacement_test);
  
  init_motor_brush(&motor_brush);
  int8_t speed = -90;
  motor_set_speed(&motor_brush, speed);
  log_printf("set motor speed to: %d\n", speed);
  while (1) {
    /* gpio_set(&motor_brush.brush.pwm_a.port, dir); */
    /* gpio_set(&motor_brush.brush.pwm_b.port, !dir); */
    //printf("tick:%ld\n", get_ticks());
    //delay_sec(1);
  }
}
