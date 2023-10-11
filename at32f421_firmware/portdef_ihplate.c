#include "portdef_ihplate.h"

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

IOPort ihplate_led = {GPIOB, GPIO_PINS_SOURCE7};
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
