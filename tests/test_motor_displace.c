#include "timer.h"
#include "motor.h"
#include "log.h"
#include "oled.h"
#include "cap_displacement.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

OLEDPort oled = {
    .en = {GPIOC, GPIO_PINS_SOURCE13},
    .i2c = {.data = {GPIOA, GPIO_PINS_SOURCE12},
            .clk = {GPIOB, GPIO_PINS_SOURCE7},
            .address = 0x78},
};

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

  log_printf("start\n");
  init_oled(&oled);
  init_motor_brush(&motor_brush);
  motor_set_speed(&motor_brush, 50);
  init_capacitor_displacement_measurement(&displacement_test);
  
  char buf[20];
  uint16_t buflen = 0;
  int8_t speed = 0;
  int8_t dir = 1;
  while (1) {
    uint32_t delay = 0;
    int32_t pos = read_displacement(&displacement_test, &delay, 1);

    if (speed >=98) dir = -1;
    else if(speed <=-98) dir = 1;
    speed += dir;
    
    snprintf(buf, 20, "spd:%d   ", speed);
    buflen = strlen(buf);
    oled_showstring(&oled, 0, 0, buf, buflen, 8, 1);
    log_printf("%s\n", buf);

    snprintf(buf, 20, "pos:%ld   ", pos);
    buflen = strlen(buf);
    oled_showstring(&oled, 0, 10, buf, buflen, 8, 1);
    log_printf("%s\n", buf);
    
    snprintf(buf, 20, "delay:%ld   ", delay);
    buflen = strlen(buf);
    oled_showstring(&oled, 0, 20, buf, buflen, 8, 1);
    log_printf("%s\n", buf);
    
    oled_refresh(&oled);
    
    motor_set_speed(&motor_brush, speed);
    delay_ms(50);
  }
}
