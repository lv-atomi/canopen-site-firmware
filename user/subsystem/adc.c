#include "adc.h"
#include "gpio.h"
#include "log.h"
#include <stdint.h>

#define MAX_ADC_PORTS 16

/* init adc, ADC1 only */
void init_adcs(ADCPort *ports, uint8_t num_ports) {
  uint8_t i;

  ASSERT(num_ports < MAX_ADC_PORTS);

  /* init adc ports */
  for (i=0; i<num_ports; i++) {
    init_gpio_analogy(&ports[i].port, GPIO_PULL_NONE, GPIO_DRIVE_STRENGTH_MODERATE);
  }

  /* ADC config */
  adc_base_config_type adc_base_struct;
  crm_periph_clock_enable(CRM_ADC1_PERIPH_CLOCK, TRUE);
  crm_adc_clock_div_set(CRM_ADC_DIV_6);
  /* nvic_irq_enable(ADC1_2_IRQn, 0, 0); */

  /* select combine mode */
#if defined (__AT32F403A_407_ADC_H)
  adc_combine_mode_select(ADC_INDEPENDENT_MODE);
#endif
  adc_base_default_para_init(&adc_base_struct);
  adc_base_struct.sequence_mode = TRUE;
  adc_base_struct.repeat_mode = TRUE;
  adc_base_struct.data_align = ADC_RIGHT_ALIGNMENT;
  adc_base_struct.ordinary_channel_length = num_ports;
  adc_base_config(ADC1, &adc_base_struct);

  for (i=0; i<num_ports; i++){
    adc_ordinary_channel_set(ADC1, ports[i].channel, i+1, ADC_SAMPLETIME_239_5); /* ch6 for voltage sensor */
  }
  /* adc_ordinary_channel_set(ADC1, ADC_CHANNEL_7, 2, ADC_SAMPLETIME_239_5); /\* ch7 for current sensor *\/ */
  /* adc_ordinary_channel_set(ADC1, ADC_CHANNEL_16, 3, ADC_SAMPLETIME_239_5); /\* ch16 for temperature sensor *\/ */
  adc_ordinary_conversion_trigger_set(ADC1, ADC12_ORDINARY_TRIG_SOFTWARE, TRUE);
  
  /* adc_tempersensor_vintrv_enable(TRUE); /\* enable internal temperature sensor *\/ */
  /* adc_voltage_monitor_enable(ADC1, ADC_VMONITOR_SINGLE_ORDINARY); */
  /* adc_voltage_monitor_threshold_value_set(ADC1, 0xBBB, 0xAAA); */
  /* adc_voltage_monitor_single_channel_select(ADC1, ADC_CHANNEL_5); */
  /* adc_interrupt_enable(ADC1, ADC_VMOR_INT, TRUE); */

  adc_enable(ADC1, TRUE);
  log_printf("ADC enabled.\n");
  adc_calibration_init(ADC1);
  log_printf("ADC calibration initialized.\n");
  while(adc_calibration_init_status_get(ADC1));
  log_printf("ADC calibration init status checked.\n");
  adc_calibration_start(ADC1);
  log_printf("ADC calibration started.\n");
  while(adc_calibration_status_get(ADC1));
  log_printf("ADC calibration completed.\n");
  
}

uint16_t * read_adcs(ADCPort * ports, uint8_t num_ports) {
  static uint16_t results[MAX_ADC_PORTS];
  uint8_t i;
  ASSERT(num_ports < MAX_ADC_PORTS);
  adc_ordinary_software_trigger_enable(ADC1, TRUE);

  for(i=0; i<num_ports; i++){
    while(adc_ordinary_software_trigger_status_get(ADC1));
    results[i] = adc_ordinary_conversion_data_get(ADC1);
  }
  return results;
}
