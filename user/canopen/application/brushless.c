#include <stdio.h>

#include "brushless.h"

OD_extension_t OD_641D_extension;
__IO uint16_t ic1value = 0;
__IO uint16_t duty_cycle = 0;
__IO uint32_t frequency = 0;

uint16_t ccr1_val = 333;
/*
 * Brushless motor pinouts
 * PB.8 -> Speed sense        TMR10_CH1
 * PB.7 -> DIR                TMR4_CH2
 * PB.6 -> Speed control      TMR4_CH1
 */

static ODR_t my_OD_read_641D(OD_stream_t *stream, void *buf,
			     OD_size_t count, OD_size_t *countRead) {
  printf("read 641D, read speed sense\n");
  *((uint16_t*)buf) = duty_cycle
  *countRead = 2;
  return ODR_OK;
}

static ODR_t my_OD_write_641D(OD_stream_t *stream, const void *buf,
			      OD_size_t count, OD_size_t *countWritten) {
  printf("write 641D, write speed\n");
  int16_t speed = *((int16_t*)buf);
  if (speed > 0) {
    ccr1_val = speed;
    /* reset PB.7 */
    GPIOB->clr = GPIO_PINS_7;     
  } else {
    ccr1_val = -speed;
    /* set PB.7 */
    GPIOB->scr = GPIO_PINS_7; 
  }
  tmr_channel_value_set(TMR4, TMR_SELECT_CHANNEL_1, ccr1_val);
  
  return ODR_OK;
}

void brushless_peripheral_init() {
  gpio_init_type gpio_init_struct;
  
  /* tmr4 clock enable */
  crm_periph_clock_enable(CRM_TMR4_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_TMR10_PERIPH_CLOCK, TRUE);

  /* gpioa clock enable */
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);
  
  gpio_default_para_init(&gpio_init_struct);

  /* PB.8 -> Speed sense */
  gpio_init_struct.gpio_pins = GPIO_PINS_8;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_mode = GPIO_MODE_INPUT;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIOB, &gpio_init_struct);

  /* PB.6 Speed control */
  gpio_init_struct.gpio_pins = GPIO_PINS_6;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_mode = GPIO_MODE_MUX;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIOB, &gpio_init_struct);

  /* PB.7 -> DIR */
  gpio_init_struct.gpio_pins = GPIO_PINS_7;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_init_struct.gpio_out_type  = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIOB, &gpio_init_struct);
  

  /* tmr4 for output */
  tmr_base_init(TMR4, 665, prescaler_value);
  tmr_cnt_dir_set(TMR4, TMR_COUNT_UP);
  tmr_clock_source_div_set(TMR4, TMR_CLOCK_DIV1);

  tmr_output_default_para_init(&tmr_oc_init_structure);
  tmr_oc_init_structure.oc_mode = TMR_OUTPUT_CONTROL_PWM_MODE_A;
  tmr_oc_init_structure.oc_idle_state = FALSE;
  tmr_oc_init_structure.oc_polarity = TMR_OUTPUT_ACTIVE_HIGH;
  tmr_oc_init_structure.oc_output_state = TRUE;
  tmr_output_channel_config(TMR4, TMR_SELECT_CHANNEL_1, &tmr_oc_init_structure);
  tmr_channel_value_set(TMR4, TMR_SELECT_CHANNEL_1, ccr1_val);
  tmr_output_channel_buffer_enable(TMR4, TMR_SELECT_CHANNEL_1, TRUE);

  tmr_period_buffer_enable(TMR4, TRUE);

  /* tmr enable counter */
  tmr_counter_enable(TMR4, TRUE);


  /* tmr10 for input */
  tmr_input_default_para_init(&tmr_ic_init_structure);
  tmr_ic_init_structure.input_filter_value = 0;
  tmr_ic_init_structure.input_channel_select = TMR_SELECT_CHANNEL_1;
  tmr_ic_init_structure.input_mapped_select = TMR_CC_CHANNEL_MAPPED_DIRECT;
  tmr_ic_init_structure.input_polarity_select = TMR_INPUT_RISING_EDGE;

  tmr_pwm_input_config(TMR10, &tmr_ic_init_structure, TMR_CHANNEL_INPUT_DIV_1);

  /* select the tmr10 input trigger: C2IF2 */
  tmr_trigger_input_select(TMR10, TMR_SUB_INPUT_SEL_C1DF1);

  /* select the sub mode: reset mode */
  tmr_sub_mode_select(TMR10, TMR_SUB_RESET_MODE);

  /* enable the sub sync mode */
  tmr_sub_sync_mode_set(TMR10, TRUE);

  /* tmr enable counter */
  tmr_counter_enable(TMR10, TRUE);

  /* enable the c1 interrupt request */
  tmr_interrupt_enable(TMR10, TMR_C1_INT, TRUE);
}

CO_ReturnError_t app_brushless_init() {
  OD_entry_t * param_641D = OD_ENTRY_H641D_brushlessMotor;
  OD_641D_extension.object = NULL;
  OD_641D_extension.read = my_OD_read_641D;
  OD_641D_extension.write = my_OD_write_641D;

  OD_extension_init(param_641D, &OD_641D_extension);

  return CO_ERROR_NO;
}

void TMR1_OVF_TMR10_IRQHandler(void)
{
  /* clear tmr3 channel interrupt pending bit */
  tmr_flag_clear(TMR10, TMR_C1_FLAG);

  /* get the input channel data value */
  ic1value = tmr_channel_value_get(TMR10, TMR_SELECT_CHANNEL_1);

  if(ic1value != 0)
  {
    /* duty cycle computation */
    duty_cycle = (tmr_channel_value_get(TMR10, TMR_SELECT_CHANNEL_1) * 100) / ic1value;

    /* frequency computation */
    frequency = system_core_clock / ic1value;
  }
  else
  {
    duty_cycle = 0;
    frequency = 0;
  }
}
