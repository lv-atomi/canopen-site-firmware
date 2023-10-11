#include "portdef_motor_direct_drive.h"
#include "at32f421.h"
#include "gpio.h"
#include "pwm.h"


IOPort mdd_led = {GPIOB, GPIO_PINS_SOURCE0};
IOPort mdd_disable = {GPIOB, GPIO_PINS_SOURCE3};
IOPort mdd_direction = {GPIOA, GPIO_PINS_SOURCE5};

MotorUnified mdd_motor = {
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

PWMPort mdd_speed_sense = {.port = {GPIOA, GPIO_PINS_SOURCE3, GPIO_MUX_0},
                      .tmr = TMR15,
                      .channel = TMR_SELECT_CHANNEL_2,
                      .complementary = FALSE};

PWMPort mdd_speed_control={.port = {GPIOA, GPIO_PINS_SOURCE4, GPIO_MUX_4},
                      .tmr = TMR14,
                      .channel = TMR_SELECT_CHANNEL_1,
                      .complementary = FALSE};

