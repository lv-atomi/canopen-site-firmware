#include "at32f421_board.h"
#include "at32f421_clock.h"
#include <stdio.h>

#include "gpio.h"
#include "slave_board_base.h"
#include "test.h"

MotorUnified motor = {
    .brush = {
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
    },
    .brushless = {
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

void gpio_config(void) {
  uint8_t i;
  init_gpio_output(&led, GPIO_OUTPUT_PUSH_PULL, GPIO_DRIVE_STRENGTH_STRONGER);
  
  for (i=0; i<2; i++){
    init_gpio_output(&gpout[i], GPIO_OUTPUT_PUSH_PULL, GPIO_DRIVE_STRENGTH_STRONGER);
    init_gpio_input(&gpin[i], GPIO_PULL_UP, GPIO_DRIVE_STRENGTH_STRONGER);
  }
}


void init_system(void){
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  system_clock_config();
  at32_board_init();
  uart_print_init(115200);  
}

void main_logic(){
  gpio_config();
  adc_config(senses, 2);
  
  delay_ms(1000);
  uint16_t position = sense_position(senses, 2);
  i2c.address += position * 2;
  i2c_config(&i2c);

  init_motor_brushless(&motor);

  printf("software_trigger_repeat\n");

  while(1) {
    adc_ordinary_software_trigger_enable(ADC1, TRUE);
    uint16_t * sensed = read_adcs(senses, 2);
    printf("sense1:%u sense2:%u\n", sensed[0], sensed[1]);
    delay_ms(1000);
  }

}

int main(void) {
  init_system();

  test_adc(senses);
  /* test_brush_motor(&motor); */
  /* test_brushless_motor(&motor); */
  /* main_logic(); */
}

