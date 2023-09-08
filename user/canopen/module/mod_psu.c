//#include "CO_app_STM32.h"
#include "app_common.h"
#include "mod_psu.h"
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
// 该数组将存储ADC读取的电压、电流和温度值
__IO uint16_t adc_valuetable[3] = {0}; /* voltage, current, temperature */
// 该数组将存储DAC设置的电压和电流值
uint16_t dac_valuetable[2] = {0}; /* voltage, current */

#define ADC_VREF                         (3.3)
#define ADC_TEMP_BASE                    (1.26)
#define ADC_TEMP_SLOPE (-0.00423)

// 为每个OD对象创建一个扩展
OD_extension_t OD_6000_extension;
OD_extension_t OD_6001_extension;
OD_extension_t OD_6002_extension;
OD_extension_t OD_6003_extension;
OD_extension_t OD_6004_extension;
OD_extension_t OD_6005_extension;

void psu_peripheral_init();


/* 把ADC/DAC读到的电压值转换成真正的数值，保留3位小数。比如返回1000则表示1.000V */
static uint32_t voltage_adda_to_numeral(uint16_t voltage_adda) {
  return voltage_adda;
}

/* 把电压值转换成ADC/DAC读到的数值范围。注意电压值保留3位小数。比如输入1000则表示1.000V */
static uint16_t voltage_numeral_to_adda(uint32_t voltage_numeral) {
  return voltage_numeral;
}

/* 把ADC/DAC读到的电流值转换成真正的数值，保留3位小数。比如返回1000则表示1.000A */
static uint32_t current_adda_to_numeral(uint16_t current_adda) {
  return current_adda;
}

/* 把电压值转换成ADC/DAC读到的数值范围。注意电压值保留3位小数。比如输入1000则表示1.000V */
static uint16_t current_numeral_to_adda(uint32_t current_numeral) {
  return current_numeral;
}

/* 把ADC/DAC读到的温度值转换成真正的数值，保留3位小数。比如返回1000则表示1.000度，注意温度值有符号，可以为负 */
static int32_t temperature_adda_to_numeral(uint16_t temperature_adda) {
  float temp = (ADC_TEMP_BASE - (double)adc_valuetable[2] * ADC_VREF / 4096) / ADC_TEMP_SLOPE + 25;
  return (int)(temp * 100);
}


/* BaseModule temperature */
// 读取模块温度
static ODR_t my_OD_read_6005(OD_stream_t *stream, void *buf,
			     OD_size_t count, OD_size_t *countRead) {

  log_printf("read 6005, internal temperature, raw adc:%u\n", adc_valuetable[2]);
  CO_setInt32(buf, temperature_adda_to_numeral(adc_valuetable[2]));
  *countRead = 4;
  return ODR_OK;
}

// 读取PSU电流
static ODR_t my_OD_read_6001(OD_stream_t *stream, void *buf, OD_size_t count, OD_size_t *countRead) {
  log_printf("read 6001, psu current, raw adc value:%u\n", adc_valuetable[1]);
  CO_setUint32(buf, current_adda_to_numeral(adc_valuetable[1]));
  *countRead = 4;
  return ODR_OK;
}

// 读取PSU电压
static ODR_t my_OD_read_6002(OD_stream_t *stream, void *buf, OD_size_t count, OD_size_t *countRead) {
  log_printf("read 6002, psu voltage, raw adc value:%u\n", adc_valuetable[0]);
  CO_setUint32(buf, voltage_adda_to_numeral(adc_valuetable[0]));
  *countRead = 4;
  return ODR_OK;
}

// 读取PSU电流设定值
static ODR_t my_OD_read_6003(OD_stream_t *stream, void *buf, OD_size_t count, OD_size_t *countRead) {
  log_printf("read 6003, psu current set, raw dac value:%u\n", dac_valuetable[1]);
  CO_setUint32(buf, current_adda_to_numeral(dac_valuetable[1]));
  *countRead = 4;
  return ODR_OK;
}

// 读取PSU电压设定值
static ODR_t my_OD_read_6004(OD_stream_t *stream, void *buf, OD_size_t count, OD_size_t *countRead) {
  log_printf("read 6004, psu voltage set: raw dac value:%u\n", dac_valuetable[0]);
  CO_setUint32(buf, voltage_adda_to_numeral(dac_valuetable[0]));
  *countRead = 4;
  return ODR_OK;
}

// 写PSU电流设定值
static ODR_t my_OD_write_6003(OD_stream_t *stream, const void *buf, OD_size_t count, OD_size_t *countWritten){
  ASSERT(*countWritten == 4);
  dac_valuetable[1] = current_numeral_to_adda(CO_getUint32(buf));
  log_printf("write 6003, psu current set: raw dac value:%u\n", dac_valuetable[1]);
  dac_2_data_set(DAC_DUAL_12BIT_RIGHT, dac_valuetable[1]);
  dac_software_trigger_generate(DAC2_SELECT);
  return ODR_OK;
}

// 写PSU电压设定值
static ODR_t my_OD_write_6004(OD_stream_t *stream, const void *buf, OD_size_t count, OD_size_t *countWritten){
  printf("write 6004, psu voltage set: raw dac value:%u\n", dac_valuetable[0]);
  ASSERT(*countWritten == 4);
  dac_valuetable[0] = voltage_numeral_to_adda(CO_getUint32(buf));
  dac_1_data_set(DAC_DUAL_12BIT_RIGHT, dac_valuetable[0]);

  dac_software_trigger_generate(DAC1_SELECT);
  return ODR_OK;
}

CO_ReturnError_t app_psu_init() {
  // 在app_psu_init()函数中，添加初始化新的OD扩展的代码
  /* OD_entry_t * param_6000 = OD_ENTRY_H6000_boardType; */
  /* OD_6000_extension.object = NULL; */
  /* OD_6000_extension.read = my_OD_read_6000; */
  /* OD_6000_extension.write = NULL; */
  /* OD_extension_init(param_6000, &OD_6000_extension); */
 
  // 初始化每个OD扩展，并设置读写回调函数
  OD_entry_t * param_6001 = OD_ENTRY_H6001_PSU_CurrentRead;
  OD_6001_extension.object = NULL;
  OD_6001_extension.read = my_OD_read_6001;
  OD_6001_extension.write = NULL;
  OD_extension_init(param_6001, &OD_6001_extension);

  OD_entry_t * param_6002 = OD_ENTRY_H6002_PSU_VoltageRead;
  OD_6002_extension.object = NULL;
  OD_6002_extension.read = my_OD_read_6002;
  OD_6002_extension.write = NULL;
  OD_extension_init(param_6002, &OD_6002_extension);

  OD_entry_t * param_6003 = OD_ENTRY_H6003_PSU_CurentSet;
  OD_6003_extension.object = NULL;
  OD_6003_extension.read = my_OD_read_6003;
  OD_6003_extension.write = my_OD_write_6003;
  OD_extension_init(param_6003, &OD_6003_extension);

  OD_entry_t * param_6004 = OD_ENTRY_H6004_PSU_VoltageSet;
  OD_6004_extension.object = NULL;
  OD_6004_extension.read = my_OD_read_6004;
  OD_6004_extension.write = my_OD_write_6004;
  OD_extension_init(param_6004, &OD_6004_extension);

  OD_entry_t * param_6005 = OD_ENTRY_H6005_PSUModuleTemperature;
  OD_6005_extension.object = NULL;
  OD_6005_extension.read = my_OD_read_6005;
  OD_6005_extension.write = NULL;
  OD_extension_init(param_6005, &OD_6005_extension);

  // 初始化外设
  psu_peripheral_init();
  return CO_ERROR_NO;
}

void psu_adc_init(){
  /* GPIO init, enable PA.6, PA.7, set to analog mode */
  gpio_init_type gpio_initstructure;
  crm_periph_clock_enable(CRM_GPIOA_PERIPH_CLOCK, TRUE);

  gpio_default_para_init(&gpio_initstructure);
  gpio_initstructure.gpio_mode = GPIO_MODE_ANALOG;
  gpio_initstructure.gpio_pins = GPIO_PINS_6 | GPIO_PINS_7;
  gpio_init(GPIOA, &gpio_initstructure);
  /* log_printf("GPIO configured for ADC.\n"); */
 
  /* DMA init */
  dma_init_type dma_init_struct;
  crm_periph_clock_enable(CRM_DMA1_PERIPH_CLOCK, TRUE);
  dma_reset(DMA1_CHANNEL1);
  dma_default_para_init(&dma_init_struct);
  dma_init_struct.buffer_size = 3;
  dma_init_struct.direction = DMA_DIR_PERIPHERAL_TO_MEMORY;
  dma_init_struct.memory_base_addr = (uint32_t)adc_valuetable;
  dma_init_struct.memory_data_width = DMA_MEMORY_DATA_WIDTH_HALFWORD;
  dma_init_struct.memory_inc_enable = TRUE;
  dma_init_struct.peripheral_base_addr = (uint32_t)&(ADC1->odt);
  dma_init_struct.peripheral_data_width = DMA_PERIPHERAL_DATA_WIDTH_HALFWORD;
  dma_init_struct.peripheral_inc_enable = FALSE;
  dma_init_struct.priority = DMA_PRIORITY_HIGH;
  dma_init_struct.loop_mode_enable = TRUE;
  dma_init(DMA1_CHANNEL1, &dma_init_struct);
  log_printf("DMA initialized.\n");
  
  dma_channel_enable(DMA1_CHANNEL1, TRUE);
  log_printf("DMA channel enabled.\n");
 
  /* ADC config */
  adc_base_config_type adc_base_struct;
  crm_periph_clock_enable(CRM_ADC1_PERIPH_CLOCK, TRUE);
  crm_adc_clock_div_set(CRM_ADC_DIV_6);
  nvic_irq_enable(ADC1_2_IRQn, 4, 0);

  /* select combine mode */
  adc_combine_mode_select(ADC_INDEPENDENT_MODE);
  adc_base_default_para_init(&adc_base_struct);
  adc_base_struct.sequence_mode = TRUE;
  adc_base_struct.repeat_mode = TRUE;
  adc_base_struct.data_align = ADC_RIGHT_ALIGNMENT;
  adc_base_struct.ordinary_channel_length = 3;
  adc_base_config(ADC1, &adc_base_struct);
  adc_ordinary_channel_set(ADC1, ADC_CHANNEL_6, 1, ADC_SAMPLETIME_239_5); /* ch6 for voltage sensor */
  adc_ordinary_channel_set(ADC1, ADC_CHANNEL_7, 2, ADC_SAMPLETIME_239_5); /* ch7 for current sensor */
  adc_ordinary_channel_set(ADC1, ADC_CHANNEL_16, 3, ADC_SAMPLETIME_239_5); /* ch16 for temperature sensor */
  adc_ordinary_conversion_trigger_set(ADC1, ADC12_ORDINARY_TRIG_SOFTWARE, TRUE);
  adc_dma_mode_enable(ADC1, TRUE);
  
  adc_tempersensor_vintrv_enable(TRUE); /* enable internal temperature sensor */
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

  adc_ordinary_software_trigger_enable(ADC1, TRUE);
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
