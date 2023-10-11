#include "motor.h"
#include "pwm.h"
#include "log.h"
#include "board.h"

MotorUnified motor = {
    .brush = {.disable = {GPIOB, GPIO_PINS_SOURCE3},
              .pwm_a =
                  {
                      .port = {GPIOA, GPIO_PINS_SOURCE8, GPIO_MUX_2},
                      .tmr = TMR1,
                      .channel = TMR_SELECT_CHANNEL_1,
                      .complementary = TRUE,
                  },
              .pwm_b =
                  {
                      .port = {GPIOA, GPIO_PINS_SOURCE7, GPIO_MUX_2},
                      .tmr = TMR1,
                      .channel = TMR_SELECT_CHANNEL_1C,
                      .complementary = TRUE,
                  }},
    .brushless = {.direction = {GPIOA, GPIO_PINS_SOURCE5},
                  .speed_sense =
                      {
                          .port = {GPIOA, GPIO_PINS_SOURCE8, GPIO_MUX_2},
                          .tmr = TMR1,
                          .channel = TMR_SELECT_CHANNEL_1,
                          .complementary = FALSE,
                      },
                  .speed_set = {
                      .port = {GPIOA, GPIO_PINS_SOURCE7, GPIO_MUX_1},
                      .tmr = TMR3,
                      .channel = TMR_SELECT_CHANNEL_2,
                      .complementary = FALSE,
                  }}};

PWMPort speed_sense = {
    .port = {GPIOA, GPIO_PINS_SOURCE8, GPIO_MUX_2},
    .tmr = TMR1,
    .channel = TMR_SELECT_CHANNEL_1,
    .complementary = FALSE,
};

PWMPort speed_set = {
    .port = {GPIOA, GPIO_PINS_SOURCE7, GPIO_MUX_1},
    .tmr = TMR3,
    .channel = TMR_SELECT_CHANNEL_2,
    .complementary = FALSE,
};


void init_motor_drive_direct(MotorUnified * motor, PWMPort * speed_set, PWMPort * speed_sense){
  ASSERT(motor);
  ASSERT(speed_set);
  ASSERT(speed_sense);

}


void init_system(void){
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  system_clock_config();
  at32_board_init();
  uart_print_init(115200);

  init_motor_brush(&motor);
  init_pwm_input(&speed_set);
  init_pwm_output(&speed_sense, 1000, 50);
}

int main(void) {
  init_system();
  
  uint32_t freq=0;
  uint8_t duty=0;
  uint8_t speed=0;
  
  while(1){
    pwm_input_freq_duty_sense(&speed_set, &freq, &duty);
    log_printf("freq:%ld, duty:%u\n", freq, duty);
    pwm_output_update_duty(&speed_sense, 30);
    motor_set_speed(&motor, speed);
    delay_ms(50);
  }
}

