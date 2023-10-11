#include <stdio.h>
#include "at32f421_gpio.h"
#include "adc.h"
#include "gpio.h"
#include "motor.h"
#include "platform_base.h"

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

int main(){
  nvic_priority_group_config(NVIC_PRIORITY_GROUP_4);
  system_clock_config();
  at32_board_init();
  uart_print_init(115200);  
  
  init_motor_brush(&motor);
  
  int8_t speed = 0;
  int8_t direction = 1;
  while (1){
    printf("current speed set:%d\n", speed);
    delay_ms(1000);
    speed += 10 * direction;
    if (speed>=100){
      direction = -1;
      speed=100;
    } else if (speed <=-100){
      speed=-100;
      direction = 1;
    }
    motor_set_speed(&motor, speed);
  }
}


