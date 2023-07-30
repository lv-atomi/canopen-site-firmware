#include "at32f421_board.h"
#include "at32f421_clock.h"
#include <stdio.h>

#include "gpio.h"
#include "motor.h"
#include "i2c.h"
#include "adc.h"

MotorBrush motor_brush = {
  .disable = {GPIOB, GPIO_PINS_3},
  .pwm_a = {
    .port = {GPIOA, GPIO_PINS_8},
    .tmr = TMR1,
    .channel = TMR_SELECT_CHANNEL_1,
    .complementary = TRUE,
  },
  .pwm_b = {
    .port = {GPIOA, GPIO_PINS_7},
    .tmr = TMR1,
    .channel = TMR_SELECT_CHANNEL_1C,
    .complementary = TRUE,
  }
};

MotorBrushless motor_brushless = {
  .direction = {GPIOA, GPIO_PINS_5},
  .speed_sense = {
    .port = {GPIOA, GPIO_PINS_8},
    .tmr = TMR1,
    .channel = TMR_SELECT_CHANNEL_1,
    .complementary = FALSE,
  },
  .speed_set = {
    .port = {GPIOA, GPIO_PINS_7},
    .tmr = TMR3,
    .channel = TMR_SELECT_CHANNEL_2,
    .complementary = FALSE,
  }
};

I2CPort i2c = {
  .clk = {GPIOB, GPIO_PINS_10},
  .data= {GPIOB, GPIO_PINS_11},
  .controller = I2C1,
  .address = 0x10,
};

ADCPort senses[2] = {
  {
    .port = {GPIOA, GPIO_PINS_0},
    .channel = ADC_CHANNEL_0,
  },
  {
    .port = {GPIOA, GPIO_PINS_1},
    .channel = ADC_CHANNEL_1,
  }
};

IOPort led = {GPIOB, GPIO_PINS_0};
IOPort gpin[2] = {{GPIOB, GPIO_PINS_6}, {GPIOB, GPIO_PINS_7}};
IOPort gpout[2] = {{GPIOA, GPIO_PINS_3}, {GPIOC, GPIO_PINS_6}};

/* i2c declaration */
#define I2C_ADDR_BASE 0x20
uint8_t i2c_addr = 0; /* slave addr */
__IO uint8_t Slave_Buffer_Rx[2];
__IO uint8_t Tx_Idx = 0, Rx_Idx = 0;
__IO uint16_t Event = 0x00;


void gpio_config(void) {
}


void adc_config(void) {
  init_adcs(senses, 2);
}

int main(void) {
  __IO uint32_t index = 0;
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  system_clock_config();
  at32_board_init();
  uart_print_init(115200);
  
  gpio_config();
  adc_config();
  printf("software_trigger_repeat\n");
  adc_ordinary_software_trigger_enable(ADC1, TRUE);
  while(1) {
    uint16_t * sensed = read_adcs(senses, 2);
    printf("sense1:%u sense2:%u\n", sensed[0], sensed[1]);
  }
}
