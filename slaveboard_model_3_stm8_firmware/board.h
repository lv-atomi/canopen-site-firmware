#ifndef BOARD_H
#define BOARD_H

#include "stm8s.h"
#include <stdint.h>

typedef struct {
  GPIO_TypeDef * port;
  GPIO_Pin_TypeDef pins;
} IOPort;

typedef struct {
  IOPort dir;
  IOPort disable_driver;
  IOPort phase_a_speedsense, phase_b;
  IOPort speed_control;
} MotorPort;

typedef struct {
  IOPort clk, data;
} I2CPort;

typedef struct {
  IOPort port;
  uint16_t channel;
} ADCPort;

void board_init();
uint16_t ADC_readonce(ADCPort *devport);
void adc_config(ADCPort *devport);
void gpoutput_config(IOPort *devport, uint8_t default_status);
void gpinput_config(IOPort *devport);
void pwm_output_config(IOPort *devport, uint32_t freq, uint8_t duty);
void pwm_input_config(IOPort *devport);

void delay_ms(uint16_t count);
uint32_t my_round(uint32_t val);
uint16_t sense_position(ADCPort *sense0, ADCPort *sense1);

#endif
