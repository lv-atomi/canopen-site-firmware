#ifndef PORTDEF_H
#define PORTDEF_H

#include "board.h"
#include "rs232.h"
#include "pwm.h"
#include "adc.h"

/*
  PB7: led        GPIO output
  PB0: IGBT_PWM   TMR3_CH3
  PB1: NOISE_DET  ADC1_IN8     (Note: not clamp to 0-3.3V)
  PB3: PULSE_DET  GPIO input   (NOTE: no ADC input)
  PA2: USART2_TX  USART2
  PA3: USART2_RX  USART2
  PA9: DEBUG_UART USART1
  PA4: IGBT_Voltage ADC1_IN4
  PA5: IGBT_Current ADC1_IN5
  PA6: Plate_Temp   ADC1_IN6
  PA7: IGBT_Temp    ADC1_IN7
*/

ADCPort IGBTVoltage = {
    .port = {GPIOA, GPIO_PINS_SOURCE4},
    .channel = ADC_CHANNEL_4,
};


ADCPort IGBTCurrent = {
    .port = {GPIOA, GPIO_PINS_SOURCE5},
    .channel = ADC_CHANNEL_5,
};

ADCPort PlateTemperature = {
    .port = {GPIOA, GPIO_PINS_SOURCE6},
    .channel = ADC_CHANNEL_6,
};

ADCPort IGBTTemperature = {
    .port = {GPIOA, GPIO_PINS_SOURCE7},
    .channel = ADC_CHANNEL_7,
};

/* ADCPort NoiseDetected = { */
/*     .port = {GPIOB, GPIO_PINS_SOURCE1}, */
/*     .channel = ADC_CHANNEL_8, */
/* }; */

PWMPort IGBTPwm = {
  .port = {GPIOB, GPIO_PINS_SOURCE0, GPIO_MUX_1},
  .channel = TMR_SELECT_CHANNEL_3,
  .tmr = TMR3,
  .complementary = FALSE};

IOPort led = {GPIOB, GPIO_PINS_SOURCE7};
IOPort NoiseDetected = {GPIOB, GPIO_PINS_SOURCE1};
IOPort PulseDetected = {GPIOB, GPIO_PINS_SOURCE3};

RS232Port comm_port = {
  .TX={GPIOA, GPIO_PINS_SOURCE2},
  .RX={GPIOA, GPIO_PINS_SOURCE3},
  .controller=USART2,
  .baud_rate=38400,
  .data_bit=USART_DATA_8BITS,
  .stop_bit=USART_STOP_1_BIT
};

#endif
