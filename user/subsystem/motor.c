#include "motor.h"
#include "log.h"

void init_motor_brushless(MotorBrushless * mbl){
  gpio_init_type gpio_initstructure;

  ASSERT(mbl);
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);

  /* gpio configuration for output pins */
  gpio_default_para_init(&gpio_initstructure);
  gpio_initstructure.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_initstructure.gpio_pull = GPIO_PULL_UP;
  gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_initstructure.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_initstructure.gpio_pins = mbl->direction.pin;
  gpio_init(mbl->direction.port, &gpio_initstructure);

  /* set timer for pwm */
  init_pwm_output(&mbl->speed_set, 20000, 50); /* 20khz, 50% */
  init_pwm_input(&mbl->speed_sense);
}

void init_motor_brush(MotorBrush * mb){
  gpio_init_type gpio_initstructure;

  ASSERT(mb);
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
  crm_periph_clock_enable(CRM_GPIOB_PERIPH_CLOCK, TRUE);

  /* gpio configuration for output pins */
  gpio_default_para_init(&gpio_initstructure);
  gpio_initstructure.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_initstructure.gpio_pull = GPIO_PULL_UP;
  gpio_initstructure.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_initstructure.gpio_mode = GPIO_MODE_OUTPUT;
  gpio_initstructure.gpio_pins = mb->disable.pin;
  gpio_init(mb->disable.port, &gpio_initstructure);
  
  gpio_initstructure.gpio_mode = GPIO_MODE_MUX;
  gpio_initstructure.gpio_pins = mb->pwm_b.port.pin;
  gpio_init(mb->pwm_b.port.port, &gpio_initstructure);

  /* set timer for pwm */
  init_pwm_output(&mb->pwm_a, 20000, 50); /* 20khz, 50% */
  init_pwm_output(&mb->pwm_b, 20000, 50); /* 20khz, 50% */
  
  /* automatic output enable, stop, dead time and lock configuration */
  tmr_brkdt_config_type tmr_brkdt_config_struct = {0};
  tmr_brkdt_default_para_init(&tmr_brkdt_config_struct);
  tmr_brkdt_config_struct.brk_enable = TRUE;
  tmr_brkdt_config_struct.auto_output_enable = TRUE;
  tmr_brkdt_config_struct.deadtime = 11;
  tmr_brkdt_config_struct.fcsodis_state = TRUE;
  tmr_brkdt_config_struct.fcsoen_state = TRUE;
  tmr_brkdt_config_struct.brk_polarity = TMR_BRK_INPUT_ACTIVE_HIGH;
  tmr_brkdt_config_struct.wp_level = TMR_WP_LEVEL_3;
  tmr_brkdt_config(TMR1, &tmr_brkdt_config_struct);
}
