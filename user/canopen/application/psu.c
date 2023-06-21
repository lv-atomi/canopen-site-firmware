#include <stdio.h>
//#include "CO_app_STM32.h"
#include "psu.h"
//#include "at32f403a_407_board.h"

/*
 * Temperature ADC1_IN16
 * PA.6 -> ADC for Voltage sense   ADC12_IN6
 * PA.7 -> ADC for Current sense   ADC12_IN7
 * PA.4 -> DAC for Voltage         DAC_OUT1
 * PA.5 -> DAC for Current         DAC_OUT2
 *
 * USE ADC1, 12bit, DMA mode for IN6 & 7 to monitor Voltage & Current
 *   and IN16 to monitor chip temperature
 * USE DAC, 12bit, for voltage * current control
 */
__IO uint16_t adc_valuetab[3] = {0}; /* voltage, current, temperature */
uint16_t dac_valuetable[2] = {0};    /* voltage, current */

OD_extension_t OD_6003_extension;
OD_extension_t OD_6412_extension;
OD_extension_t OD_6413_extension;
OD_extension_t OD_6414_extension;
OD_extension_t OD_6415_extension;

void psu_peripheral_init();

/* BaseModule temperature */
static ODR_t my_OD_read_6003(OD_stream_t *stream, void *buf,
			     OD_size_t count, OD_size_t *countRead) {
  
  printf("read 6003, internal temperature\n");
  *((uint16_t *)buf) = adc_valuetab[2];
  *countRead = 2;
  return ODR_OK;
}


/* PSU current read */
static ODR_t my_OD_read_6412(OD_stream_t *stream, void *buf,
			     OD_size_t count, OD_size_t *countRead) {
  
  printf("read 6412, psu current\n");
  *((uint16_t *)buf) = adc_valuetab[1];
  *countRead = 2;
  return ODR_OK;
}

/* PSU voltage read */
static ODR_t my_OD_read_6413(OD_stream_t *stream, void *buf,
			     OD_size_t count, OD_size_t *countRead) {
  printf("read 6413, psu voltage\n");
  *((uint16_t *)buf) = adc_valuetab[0];
  *countRead = 2;

  return ODR_OK;
}

/* PSU current set read */
static ODR_t my_OD_read_6414(OD_stream_t *stream, void *buf,
			     OD_size_t count, OD_size_t *countRead) {
  printf("read 6414, psu current set\n");
  *((uint16_t *)buf) = dac_valuetable[1];
  *countRead = 2;
  return ODR_OK;
}

/* PSU voltage set read */
static ODR_t my_OD_read_6415(OD_stream_t *stream, void *buf,
			     OD_size_t count, OD_size_t *countRead) {
  printf("read 6415, psu voltage set\n");
  *((uint16_t *)buf) = dac_valuetable[0];
  *countRead = 2;
  return ODR_OK;
}

/* PSU current set write */
static ODR_t my_OD_write_6414(OD_stream_t *stream, const void *buf,
			      OD_size_t count, OD_size_t *countWritten){
  printf("write 6414, psu voltage set\n");
  dac_valuetable[1] = *((uint16_t *)buf);
  dac_2_data_set(DAC_DUAL_12BIT_RIGHT, dac_valuetable[1]);
  return ODR_OK;
}

/* PSU voltage set write */
static ODR_t my_OD_write_6415(OD_stream_t *stream, const void *buf,
			      OD_size_t count, OD_size_t *countWritten){
  printf("write 6415, psu current set\n");
  dac_valuetable[0] = *((uint16_t *)buf);
  dac_1_data_set(DAC_DUAL_12BIT_RIGHT, dac_valuetable[0]);
  return ODR_OK;
}

CO_ReturnError_t app_psu_init() {
  OD_entry_t * param_6003 = OD_ENTRY_H6003_moduleTemperature;
  OD_6003_extension.object = NULL;
  OD_6003_extension.read = my_OD_read_6003;
  OD_6003_extension.write = NULL;

  OD_extension_init(param_6003, &OD_6003_extension);

  OD_entry_t * param_6412 = OD_ENTRY_H6412_PSU_CurrentRead;
  OD_6412_extension.object = NULL;
  OD_6412_extension.read = my_OD_read_6412;
  OD_6412_extension.write = NULL;

  OD_extension_init(param_6412, &OD_6412_extension);

  OD_entry_t * param_6413 = OD_ENTRY_H6413_PSU_VoltageRead;
  OD_6413_extension.object = NULL;
  OD_6413_extension.read = my_OD_read_6413;
  OD_6413_extension.write = NULL;

  OD_extension_init(param_6413, &OD_6413_extension);

  OD_entry_t * param_6414 = OD_ENTRY_H6414_PSU_CurentSet;
  OD_6414_extension.object = NULL;
  OD_6414_extension.read = my_OD_read_6414;
  OD_6414_extension.write = my_OD_write_6414;

  OD_extension_init(param_6414, &OD_6414_extension);

  OD_entry_t * param_6415 = OD_ENTRY_H6415_PSU_VoltageSet;
  OD_6415_extension.object = NULL;
  OD_6415_extension.read = my_OD_read_6415;
  OD_6415_extension.write = my_OD_write_6415;

  OD_extension_init(param_6415, &OD_6415_extension);

  psu_peripheral_init();
}

void psu_adc_init(){
  /* GPIO init, enable PA.6, PA.7, set to analog mode */
  gpio_init_type gpio_initstructure;
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);

  gpio_default_para_init(&gpio_initstructure);
  gpio_initstructure.gpio_mode = GPIO_MODE_ANALOG;
  gpio_initstructure.gpio_pins = GPIO_PINS_6 | GPIO_PINS_7;
  gpio_init(GPIOA, &gpio_initstructure);

  /* DMA init */
  dma_init_type dma_init_struct;
  crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);
  dma_reset(DMA1_CHANNEL1);
  dma_default_para_init(&dma_init_struct);
  dma_init_struct.buffer_size = 3;
  dma_init_struct.direction = DMA_DIR_PERIPHERAL_TO_MEMORY;
  dma_init_struct.memory_base_addr = (uint32_t)adc_valuetab;
  dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_HALFWORD;
  dma_init_struct.memory_inc_enable = TRUE;
  dma_init_struct.peripheral_base_addr = (uint32_t)&(ADC1->odt);
  dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_HALFWORD;
  dma_init_struct.peripheral_inc_enable = FALSE;
  dma_init_struct.priority = DMA_PRIORITY_HIGH;
  dma_init_struct.loop_mode_enable = TRUE;
  dma_init(DMA1_CHANNEL1, &dma_init_struct);

  dma_channel_enable(DMA1_CHANNEL1, TRUE);

  /* ADC config */
  adc_base_config_type adc_base_struct;
  crm_periph_clock_enable(CRM_ADC1_PERIPH_CLOCK, TRUE);
  crm_adc_clock_div_set(CRM_ADC_DIV_6);
  nvic_irq_enable(ADC1_2_IRQn, 0, 0);

  /* select combine mode */
  adc_combine_mode_select(ADC_INDEPENDENT_MODE);
  adc_base_default_para_init(&adc_base_struct);
  adc_base_struct.sequence_mode = TRUE;
  adc_base_struct.repeat_mode = FALSE;
  adc_base_struct.data_align = ADC_RIGHT_ALIGNMENT;
  adc_base_struct.ordinary_channel_length = 3;
  adc_base_config(ADC1, &adc_base_struct);
  adc_ordinary_channel_set(ADC1, ADC_CHANNEL_6, 1, ADC_SAMPLETIME_239_5); /* ch6 for voltage sensor */
  adc_ordinary_channel_set(ADC1, ADC_CHANNEL_7, 2, ADC_SAMPLETIME_239_5); /* ch7 for current sensor */
  adc_ordinary_channel_set(ADC1, ADC_CHANNEL_16, 3, ADC_SAMPLETIME_239_5); /* ch16 for temperature sensor */
  adc_ordinary_conversion_trigger_set(ADC1, ADC12_ORDINARY_TRIG_SOFTWARE, TRUE);
  adc_dma_mode_enable(ADC1, TRUE);
  adc_tempersensor_vintrv_enable(TRUE); /* enable internal temperature sensor */
  adc_voltage_monitor_enable(ADC1, ADC_VMONITOR_SINGLE_ORDINARY);
  adc_voltage_monitor_threshold_value_set(ADC1, 0xBBB, 0xAAA);
  adc_voltage_monitor_single_channel_select(ADC1, ADC_CHANNEL_5);
  adc_interrupt_enable(ADC1, ADC_VMOR_INT, TRUE);

  adc_enable(ADC1, TRUE);
  adc_calibration_init(ADC1);
  while(adc_calibration_init_status_get(ADC1));
  adc_calibration_start(ADC1);
  while(adc_calibration_status_get(ADC1));
}

void psu_dac_init(){
  /* GPIO init, enable PA.4, PA.5, set to analog mode */
  gpio_init_type gpio_init_struct;
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);
  
  /* once the dac is enabled, the corresponding gpio pin is automatically
     connected to the dac converter. in order to avoid parasitic consumption,
     the gpio pin should be configured in analog */
  gpio_default_para_init(&gpio_init_struct);
  gpio_init_struct.gpio_mode = GPIO_MODE_ANALOG;
  gpio_init_struct.gpio_pins = GPIO_PINS_4 | GPIO_PINS_5;
  gpio_init_struct.gpio_out_type = GPIO_OUTPUT_PUSH_PULL;
  gpio_init_struct.gpio_pull = GPIO_PULL_NONE;
  gpio_init_struct.gpio_drive_strength = GPIO_DRIVE_STRENGTH_STRONGER;
  gpio_init(GPIOA, &gpio_init_struct);

  crm_periph_clock_enable(CRM_DAC_PERIPH_CLOCK, TRUE);

  dac_trigger_select(DAC1_SELECT, DAC_SOFTWARE_TRIGGER);
  dac_trigger_select(DAC2_SELECT, DAC_SOFTWARE_TRIGGER);
  
  dac_trigger_enable(DAC1_SELECT, TRUE);
  dac_trigger_enable(DAC2_SELECT, TRUE);
  
  /* enable dac1: once the dac1 is enabled, pa.04 is
     automatically connected to the dac converter. */
  dac_enable(DAC1_SELECT, TRUE);

  /* enable dac2: once the dac2 is enabled, pa.05 is
     automatically connected to the dac converter. */
  dac_enable(DAC2_SELECT, TRUE);
  
  dac_dual_data_set(DAC_DUAL_12BIT_RIGHT, dac_valuetable[0], dac_valuetable[1]);
}

void psu_peripheral_init() {
  psu_adc_init();
  psu_dac_init();
}

void app_psu_async(){
  
}
