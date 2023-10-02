#include "motor.h"
#include "gpio.h"
#include "log.h"
#include "pwm.h"

void init_motor_brushless(MotorUnified * motor){
  ASSERT(motor);

  motor->is_brushless = 1;

  /* gpio configuration for output pins */
  init_gpio_output(&motor->brush.disable,
		   GPIO_OUTPUT_PUSH_PULL,
		   GPIO_DRIVE_STRENGTH_STRONGER);
  /* disable hbridge */
  gpio_set(&motor->brush.disable, 1);

  /* gpio configuration for output pins */
  init_gpio_output(&motor->brushless.direction,
		   GPIO_OUTPUT_PUSH_PULL,
		   GPIO_DRIVE_STRENGTH_STRONGER);

  /* set timer for pwm */
  init_pwm_output(&motor->brushless.speed_set, 20000, 50); /* 20khz, 50% */
  init_pwm_input(&motor->brushless.speed_sense);
}

void init_motor_brush(MotorUnified * motor){
  ASSERT(motor);

  motor->is_brushless = 0;
  /* gpio configuration for output pins */
  init_gpio_output(&motor->brush.disable,
		   GPIO_OUTPUT_PUSH_PULL,
		   GPIO_DRIVE_STRENGTH_MODERATE);
  /* enable hbridge */
  gpio_set(&motor->brush.disable, 0);

  /* set timer for pwm */
  init_pwm_output(&motor->brush.pwm_a, 15000, 50); /* 50khz, 50% */
  init_pwm_output(&motor->brush.pwm_b, 15000, 50); /* 50khz, 50% */
  
}

void motor_set_speed(MotorUnified *motor, int8_t speed){
  ASSERT(motor);
  if (motor->is_brushless){
    if (speed > 0) {
      gpio_set(&motor->brushless.direction, 0);
      pwm_output_update_duty(&motor->brushless.speed_set, speed);
    } else if (speed < 0) {
      pwm_output_update_duty(&motor->brushless.speed_set, -speed);
      gpio_set(&motor->brushless.direction, 1);
    } else {
      pwm_output_update_duty(&motor->brushless.speed_set, 0);
    }
  } else {
    //log_printf("set motor speed:%d\n", speed);
    pwm_output_update_duty(&motor->brush.pwm_a, 50 + speed/2);
  }
  
}

uint8_t motor_speed_sense(MotorUnified *motor){
  uint32_t freq;
  uint8_t duty;
  ASSERT(motor->is_brushless);
  pwm_input_freq_duty_sense(&motor->brushless.speed_sense, &freq, &duty);
  return duty;
}
