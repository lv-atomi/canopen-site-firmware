#ifndef PORTDEF_IHPLATE_H
#define PORTDEF_IHPLATE_H

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
extern ADCPort IGBTVoltage;
extern ADCPort IGBTCurrent;
extern ADCPort PlateTemperature;
extern ADCPort IGBTTemperature;

extern PWMPort IGBTPwm;

extern IOPort ihplate_led;
extern IOPort NoiseDetected;
extern IOPort PulseDetected;

extern RS232Port comm_port;

#endif
