#include "at32f421_board.h"
#include "at32f421_clock.h"
#include <stdio.h>

#include "gpio.h"
#include "motor.h"
#include "i2c.h"
#include "adc.h"

MotorBrush motor_brush = {
  .disable = {GPIOB, GPIO_PIN_3},
  .pwm_a = {
    .port = {GPIOA, GPIO_PIN_8},
    .tmr = TMR1,
    .channel = TMR_SELECT_CHANNEL_1,
    .complementary = TRUE,
  }
  .pwm_b = {
    .port = {GPIOA, GPIO_PIN_7},
    .tmr = TMR1,
    .channel = TMR_SELECT_CHANNEL_1C,
    .complementary = TRUE,
  }
};

MotorBrushless motor_brushless = {
  .direction = {GPIOA, GPIO_PIN_5},
  .speed_sense = {
    .port = {GPIOA, GPIO_PIN_8},
    .tmr = TMR1,
    .channel = TMR_SELECT_CHANNEL_1,
    .complementary = FALSE,
  },
  .speed_set = {
    .port = {GPIOA, GPIO_PIN_7},
    .tmr = TMR3,
    .channel = TMR_SELECT_CHANNEL_2,
    .complementary = FALSE,
  }
};

I2CPort i2c = {
  .clk = {GPIOB, GPIO_PIN_10},
  .data= {GPIOB, GPIO_PIN_11},
};

ADCPort sense1 = {
    .port = {GPIOA, GPIO_PIN_0},
    .channel = 0,
};

ADCPort sense2 = {
    .port = {GPIOA, GPIO_PIN_1},
    .channel = 1,
};

IOPort led = {GPIOB, GPIO_PIN_0};
IOPort gpin[2] = {{GPIOB, GPIO_PIN_6}, {GPIOB, GPIO_PIN_7}};
IOPort gpout[2] = {{GPIOA, GPIO_PIN_3}, {GPIOC, GPIO_PIN_6}};

/* i2c declaration */
#define I2C_ADDR_BASE 0x20
uint8_t i2c_addr = 0; /* slave addr */
__IO uint8_t Slave_Buffer_Rx[2];
__IO uint8_t Tx_Idx = 0, Rx_Idx = 0;
__IO uint16_t Event = 0x00;


static void gpio_config(void)
{
  gpio_init_type gpio_initstructure;
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);

  gpio_default_para_init(&gpio_initstructure);
  gpio_initstructure.gpio_mode = GPIO_MODE_ANALOG;
  gpio_initstructure.gpio_pins = GPIO_PINS_4 | GPIO_PINS_5 | GPIO_PINS_6;
  gpio_init(GPIOA, &gpio_initstructure);
}


static void adc_config(void)
{
  adc_base_config_type adc_base_struct;
  crm_periph_clock_enable(CRM_ADC1_PERIPH_CLOCK, TRUE);
  crm_adc_clock_div_set(CRM_ADC_DIV_6);

  adc_base_default_para_init(&adc_base_struct);
  adc_base_struct.sequence_mode = TRUE;
  adc_base_struct.repeat_mode = TRUE;
  adc_base_struct.data_align = ADC_RIGHT_ALIGNMENT;
  adc_base_struct.ordinary_channel_length = 3;
  adc_base_config(ADC1, &adc_base_struct);
  adc_ordinary_channel_set(ADC1, ADC_CHANNEL_4, 1, ADC_SAMPLETIME_239_5);
  adc_ordinary_channel_set(ADC1, ADC_CHANNEL_5, 2, ADC_SAMPLETIME_239_5);
  adc_ordinary_channel_set(ADC1, ADC_CHANNEL_6, 3, ADC_SAMPLETIME_239_5);
  adc_ordinary_conversion_trigger_set(ADC1, ADC12_ORDINARY_TRIG_SOFTWARE, TRUE);
  adc_dma_mode_enable(ADC1, TRUE);

  adc_enable(ADC1, TRUE);
  adc_calibration_init(ADC1);
  while(adc_calibration_init_status_get(ADC1));
  adc_calibration_start(ADC1);
  while(adc_calibration_status_get(ADC1));
}

int main(void)
{
  __IO uint32_t index = 0;
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  system_clock_config();
  at32_board_init();
  uart_print_init(115200);
  gpio_config();
  adc_config();
  printf("software_trigger_repeat \r\n");
  printf("please_debug_check_data_and_conversion_times \r\n");
  adc_ordinary_software_trigger_enable(ADC1, TRUE);
  while(1)
  {
  }
}
