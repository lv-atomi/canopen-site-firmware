#include "motor.h"
#include "gpio.h"
#include "log.h"

void init_motor_brushless(MotorBrushless * mbl){
  ASSERT(mbl);

  /* gpio configuration for output pins */
  init_gpio_output(&mbl->direction,
		   GPIO_OUTPUT_PUSH_PULL,
		   GPIO_MODE_OUTPUT,
		   GPIO_DRIVE_STRENGTH_STRONGER);

  /* set timer for pwm */
  init_pwm_output(&mbl->speed_set, 20000, 50); /* 20khz, 50% */
  init_pwm_input(&mbl->speed_sense);
}

void init_motor_brush(MotorBrush * mb){
  gpio_init_type gpio_initstructure;

  ASSERT(mb);
  /* gpio configuration for output pins */
  init_gpio_output(&mb->disable,
		   GPIO_OUTPUT_PUSH_PULL,
		   GPIO_MODE_OUTPUT,
		   GPIO_DRIVE_STRENGTH_STRONGER);

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
